import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

from test_optional_plt import fixture


TLS_LOAD = bytes.fromhex("66 66 66 64 48 8b 04 25 00 00 00 00")


def register_load(register):
    high, low = register >> 3, register & 7
    extension = b"\x41" if high else b""
    save = extension + bytes([0x50 + low]) if register else b""
    restore = extension + bytes([0x58 + low]) if register else b""
    prefix = save + bytes.fromhex("b8 07 00 00 00 b9 05 00 00 00")
    load = bytes([0x64, 0x48 | high << 2, 0x8b, 0x04 | low << 3, 0x25, 0, 0, 0, 0])
    rest = extension + bytes([0x8b, 0x40 | low]) + (b"\x24" if low == 4 else b"") + b"\xf0" + restore + b"\xc3"
    if register != 1:
        rest = bytes.fromhex("83 f9 05 75") + bytes([len(rest)]) + rest
    if register != 0:
        rest = bytes.fromhex("83 f8 07 75") + bytes([len(rest)]) + rest
    return len(prefix), prefix + load + rest + restore + bytes.fromhex("31 c0 c3")


ALU_READ = {"add": 0x03, "or": 0x0b, "adc": 0x13, "sbb": 0x1b, "and": 0x23, "sub": 0x2b, "xor": 0x33, "cmp": 0x3b}


def fs_alu(opcode, register, displacement):
    return bytes([0x64, 0x48 | (register >> 3) << 2, opcode, 0x04 | (register & 7) << 3, 0x25]) + struct.pack("<i", displacement)


def alu_check_body(opcode, register, base, value, result, flags_masked):
    code = bytearray()
    jumps = []

    def emit(data):
        code.extend(data)

    def move_immediate(target, immediate):
        emit(bytes([0x48 | (target >> 3), 0xb8 | (target & 7)]) + struct.pack("<Q", immediate & ((1 << 64) - 1)))

    def compare_with_r8(target):
        emit(bytes([0x48 | 0x44 | (0x01 if target >= 8 else 0), 0x39, 0xc0 | (target & 7)]))

    def fail_unless_equal():
        emit(bytes.fromhex("0f 85 00 00 00 00"))
        jumps.append(len(code) - 4)

    emit(bytes.fromhex("64 c7 04 25 28 00 00 00") + struct.pack("<I", value & 0xffffffff))
    for target in (0, 1, 3):
        move_immediate(target, base)
    emit(fs_alu(opcode, register, 0x28))
    emit(bytes.fromhex("9c 5a 83 e2 41"))
    move_immediate(8, result)
    compare_with_r8(register)
    fail_unless_equal()
    for target in (0, 1, 3):
        if target == register:
            continue
        move_immediate(8, base)
        compare_with_r8(target)
        fail_unless_equal()
    emit(bytes.fromhex("83 fa") + bytes([flags_masked]))
    fail_unless_equal()
    emit(bytes.fromhex("b8 2a 00 00 00 c3 b8 01 00 00 00 c3"))
    failure = len(code) - 6
    for position in jumps:
        struct.pack_into("<i", code, position, failure - position - 4)
    return bytes(code)


def alu_execution_cases():
    mask = (1 << 64) - 1
    cases = (
        ("xor", 0x33, lambda a, b: a ^ b, 0xffffffff00000000, 0xffffffff),
        ("and", 0x23, lambda a, b: a & b, 0xffffffff00000000, 0xffffffff),
        ("or", 0x0b, lambda a, b: a | b, 0xffffffff00000000, 0xffffffff),
        ("add", 0x03, lambda a, b: a + b, 0x0102030405060708, 0x11223344),
        ("sub", 0x2b, lambda a, b: a - b, 0x0000000012345678, 0xffffffff),
        ("cmp", 0x3b, lambda a, b: a, 0x0000000012345678, 0xffffffff),
        ("cmp-equal", 0x3b, lambda a, b: a, 0x00000000ffffffff, 0xffffffff),
    )
    for name, opcode, operation, base, value in cases:
        result = operation(base, value) & mask
        if opcode in (0x2b, 0x3b):
            zero = 1 if (base - value) & mask == 0 else 0
            carry = 1 if base < value else 0
        elif opcode == 0x03:
            zero = 1 if (base + value) & mask == 0 else 0
            carry = 1 if base + value > mask else 0
        else:
            zero = 1 if result == 0 else 0
            carry = 0
        flags_masked = (zero << 6) | carry
        for register in (0, 1, 3):
            body = alu_check_body(opcode, register, base, value, result, flags_masked)
            yield f"alu-exec-{name}-{register}", make_image("register", "unwind", body=body)


def make_image(transfer, metadata, extent=None, body=None):
    image = fixture()
    image.extend(b"\x90" * 0x1000)
    struct.pack_into("<Q", image, 24, 0x1200)
    struct.pack_into("<I", image, 68, 6)
    image[0x1200:0x1300] = b"\x90" * 0x100
    target = 0x1240
    if transfer == "table":
        code = bytes.fromhex("31 c0 48 8d 0d") + struct.pack("<i", 0x780 - 0x1209)
        code += bytes.fromhex("48 63 04 81 48 01 c8 ff e0")
        struct.pack_into("<i", image, 0x780, target - 0x780)
    elif transfer == "register":
        code = bytes.fromhex("48 8d 05") + struct.pack("<i", target - 0x1207)
        code += bytes.fromhex("ff e0")
    elif transfer == "memory":
        code = bytes.fromhex("48 8d 0d") + struct.pack("<i", target - 0x1207)
        code += bytes.fromhex("48 89 4c 24 f8 ff 64 24 f8")
    else:
        raise ValueError(transfer)
    image[0x1200:0x1200 + len(code)] = code
    if body is None:
        body = TLS_LOAD + bytes.fromhex("8b 40 f0 c3")
    image[target:target + len(body)] = body
    image[0x1850:0x1850 + len(TLS_LOAD)] = TLS_LOAD
    struct.pack_into("<QQq", image, 0x700, 0x300, 8, 0x1200)
    struct.pack_into("<Q", image, 0x800, 42)
    struct.pack_into("<H", image, 56, 4 if metadata == "symbol" else 5)
    struct.pack_into("<IIQQQQQQ", image, 176, 7, 4, 0x800, 0x800, 0x800, 8, 16, 16)
    struct.pack_into("<IIQQQQQQ", image, 232 if metadata == "symbol" else 288, 1, 5, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000)
    function_size = target + len(body) - 0x1200 if extent is None else extent
    if metadata == "unwind":
        struct.pack_into("<IIQQQQQQ", image, 232, 0x6474e550, 4, 0x980, 0x980, 0x980, 32, 32, 8)
        struct.pack_into("<II", image, 0x900, 12, 0)
        image[0x908:0x910] = bytes.fromhex("01 00 01 78 10 00 00 00")
        struct.pack_into("<IIQQ", image, 0x910, 20, 0x14, 0x1200, function_size)
        struct.pack_into("<BBBBQIQQ", image, 0x980, 1, 0, 3, 0, 0x900, 1, 0x1200, 0x910)
    elif metadata == "symbol":
        struct.pack_into("<IBBHQQ", image, 0x638, 0, 0x12, 0, 1, 0x1200, function_size)
        struct.pack_into("<IIII", image, 0x680, 1, 2, 1, 0)
        struct.pack_into("<qQqQ", image, 0x470, 4, 0x680, 0, 0)
        struct.pack_into("<QQ", image, 160, 0x90, 8)
        struct.pack_into("<Q", image, 152, 0x90)
    else:
        raise ValueError(metadata)
    return image


def pe_bytes_at(pe, rva, size):
    header = struct.unpack_from("<I", pe, 0x3c)[0]
    count = struct.unpack_from("<H", pe, header + 6)[0]
    sections = header + 24 + struct.unpack_from("<H", pe, header + 20)[0]
    for index in range(count):
        offset = sections + index * 40
        address, length, position = struct.unpack_from("<III", pe, offset + 12)
        if address <= rva and rva + size <= address + length:
            return pe[position + rva - address:position + rva - address + size]
    raise AssertionError(f"Unmapped PE RVA {rva:#x}")


def main():
    relinker = Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory(prefix="anyps5-tls-coverage-") as directory:
        work = Path(directory)

        def convert(name, image, error=None, tls_address=0x1240):
            source = work / (name + ".elf")
            output = source.with_suffix(".exe")
            source.write_bytes(image)
            result = subprocess.run([str(relinker), "--skip-sce-module", "--windows", str(source), str(output)],
                                    capture_output=True, text=True, timeout=30)
            if error is not None:
                assert result.returncode == 2 and error in result.stderr and not output.exists(), result
                return
            assert result.returncode == 0, (name, result.stdout, result.stderr)
            pe = output.read_bytes()
            assert pe_bytes_at(pe, 0x10000 + tls_address, 1) == b"\xe9", name
            assert pe_bytes_at(pe, 0x11850, len(TLS_LOAD)) == TLS_LOAD, name
            if os.name == "nt":
                executed = subprocess.run([str(output)], capture_output=True, timeout=30)
                assert executed.returncode == 42, (name, executed.returncode, executed.stderr)

        for metadata in ("unwind", "symbol"):
            for transfer in ("table", "register", "memory"):
                convert(metadata + "-" + transfer, make_image(transfer, metadata))
            convert(metadata + "-truncated", make_image("register", metadata, 0x46), "Code analysis:")
            convert(metadata + "-outside", make_image("register", metadata, 0x1000), "Code analysis: function exceeds executable segment")
        overlapping = make_image("register", "unwind")
        overlapping[0x1200:0x1205] = b"\xe9" + struct.pack("<i", 0x1245 - 0x1205)
        convert("overlapping-entry", overlapping, "Branch enters a guest TLS instruction")
        harmless_overlap = make_image("register", "unwind")
        harmless_overlap[0x1210:0x1214] = bytes.fromhex("66 90 eb fd")
        convert("overlapping-nontls-stream", harmless_overlap)
        external = make_image("register", "unwind")
        external[0x1300:0x1310] = external[0x1240:0x1250]
        external[0x1240:0x1250] = b"\xe8" + struct.pack("<i", 0x1300 - 0x1245) + b"\xc3" + b"\x90" * 10
        convert("direct-call-from-indirect-block", external, tls_address=0x1300)
        for register in range(16):
            offset, body = register_load(register)
            image = make_image("register", "unwind", body=body)
            if register == 4:
                convert("load-register-4", image, "Unsupported Windows guest TLS instruction")
            else:
                convert("load-register-" + str(register), image, tls_address=0x1240 + offset)
        for name, opcode in ALU_READ.items():
            for register in (0, 1, 2, 3, 8, 13):
                for displacement in (0, 40, -8):
                    body = fs_alu(opcode, register, displacement) + b"\xc3"
                    case = f"alu-{name}-{register}-{displacement}"
                    image = make_image("register", "unwind", body=body)
                    source = work / (case + ".elf")
                    output = source.with_suffix(".exe")
                    source.write_bytes(image)
                    result = subprocess.run([str(relinker), "--skip-sce-module", "--windows", str(source), str(output)],
                                            capture_output=True, text=True, timeout=30)
                    assert result.returncode == 0, (case, result.stderr)
                    pe = output.read_bytes()
                    patched_address = 0x10000 + 0x1240
                    patched = pe_bytes_at(pe, patched_address, 5)
                    assert patched[0] == 0xe9, case
                    stub_address = patched_address + 5 + struct.unpack_from("<i", patched, 1)[0]
                    stub = pe_bytes_at(pe, stub_address, 96)
                    if register == 0:
                        expected = bytes.fromhex("48 8b 90") + struct.pack("<i", displacement) + bytes([0x58, 0x48, opcode, 0xc2])
                    elif register == 1:
                        expected = bytes.fromhex("48 8b 80") + struct.pack("<i", displacement) + bytes.fromhex("48 8b 4c 24 08") + bytes([0x48, opcode, 0xc8])
                    else:
                        expected = bytes.fromhex("48 8b 80") + struct.pack("<i", displacement) + bytes([0x48 | ((register >> 3) << 2), opcode, 0xc0 | ((register & 7) << 3)])
                    assert expected in stub, (case, expected.hex(), stub.hex())
        for name, image in alu_execution_cases():
            convert(name, image)
        convert("rsp-alu", make_image("register", "unwind", body=fs_alu(0x33, 4, 40) + b"\xc3"),
                "Unsupported Windows guest TLS instruction")
        conflicting = make_image("register", "symbol")
        unwind = make_image("register", "unwind", 0x51)
        conflicting[0x900:0x9a0] = unwind[0x900:0x9a0]
        conflicting[232:344] = unwind[232:344]
        struct.pack_into("<H", conflicting, 56, 5)
        convert("conflicting-function-extents", conflicting, "Code analysis: conflicting function ranges")
    print("TLS function coverage integration tests passed")


if __name__ == "__main__":
    main()
