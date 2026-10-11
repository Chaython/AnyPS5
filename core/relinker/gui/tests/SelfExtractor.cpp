#include "../SelfExtractor.hpp"
#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
void Put(std::vector<std::uint8_t>& b, std::size_t off, std::uint64_t v, int bytes) {
    for (int i = 0; i < bytes; ++i) b.at(off + i) = std::uint8_t(v >> (i * 8));
}
void Check(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
std::vector<std::uint8_t> MakeSelf(std::uint64_t dataProperties = 0x2804) {
    std::vector<std::uint8_t> b(0x147, 0);
    Put(b, 0, 0x1d3d154f, 4);
    b[4] = 0; b[6] = 1;
    Put(b, 16, 0x144, 8);
    Put(b, 24, 2, 2);
    Put(b, 32, 0x110004, 8);
    Put(b, 40, 0x120, 8);
    Put(b, 48, 0, 8);
    Put(b, 64, dataProperties, 8);
    Put(b, 72, 0x140, 8);
    Put(b, 80, 4, 8);
    Put(b, 88, 4, 8);
    const auto elf = std::size_t{0x60};
    Put(b, elf, 0x464c457f, 4);
    b[elf+4] = 2; b[elf+5] = 1; b[elf+6] = 1;
    Put(b, elf+16, 0xfe10, 2);
    Put(b, elf+18, 62, 2);
    Put(b, elf+20, 1, 4);
    Put(b, elf+32, 64, 8);
    Put(b, elf+52, 64, 2);
    Put(b, elf+54, 56, 2);
    Put(b, elf+56, 2, 2);
    Put(b, elf+64, 1, 4);
    Put(b, elf+64+8, 0x200, 8);
    Put(b, elf+64+32, 4, 8);
    Put(b, elf+64+40, 4, 8);
    Put(b, elf+120, 4, 4);
    Put(b, elf+120+8, 0x204, 8);
    Put(b, elf+120+32, 3, 8);
    Put(b, elf+120+40, 3, 8);
    b[0x140]='E'; b[0x141]='L'; b[0x142]='F'; b[0x143]='!';
    b[0x144]='X'; b[0x145]='Y'; b[0x146]='Z';
    return b;
}
void Write(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes) {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    Check(static_cast<bool>(out), "fixture write failed");
}
bool Rejects(const std::filesystem::path& source, const std::filesystem::path& output) {
    try { GuiSelf::Extract(source, output); } catch (const std::runtime_error&) { return true; }
    return false;
}
}
int main() {
    namespace fs = std::filesystem;
    const auto root = fs::temp_directory_path() /
        ("anyps5-self-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(root);
    try {
        const auto source = root / "libc.prx", output = root / "output.elf";
        Write(source, MakeSelf());
        Check(GuiSelf::Detect(source) == GuiSelf::Kind::Self, "SELF detection failed");
        GuiSelf::Extract(source, output);
        Check(GuiSelf::Detect(output) == GuiSelf::Kind::Elf, "ELF detection failed");
        {
            std::ifstream file(output, std::ios::binary | std::ios::ate);
            Check(file.tellg() == std::streampos(0x207), "ELF file length incorrect");
            std::array<char, 7> bytes{};
            file.seekg(0x200);
            file.read(bytes.data(), bytes.size());
            Check(std::string(bytes.data(), bytes.size()) == "ELF!XYZ", "ELF segments/tail missing");
        }
        Check(fs::file_size(source) == 0x147, "source was modified");
        Write(source, MakeSelf(0x2806));
        Check(Rejects(source, output), "encrypted SELF was accepted");
        Write(source, MakeSelf(0x280c));
        Check(Rejects(source, output), "compressed SELF was accepted");
        auto bad = MakeSelf();
        Put(bad, 72, 0x1000, 8);
        Write(source, bad);
        Check(Rejects(source, output), "out-of-range SELF segment was accepted");
        fs::remove_all(root);
        return 0;
    } catch (...) {
        fs::remove_all(root);
        throw;
    }
}
