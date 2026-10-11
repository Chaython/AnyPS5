# Windows graphical converter (fork)

This optional, Windows-only GUI is a frontend for the upstream AnyPS5 **relinker**, not a replacement or emulator.

## Usage
1. Download a Windows GUI bundle from this fork's **Windows GUI Bundle** workflow artifacts, or build from source.
2. Keep `AnyPS5.exe` and `relinker.exe` together. Keep the `libs` folder from the bundle; it contains open-source runtime providers needed by translated executables.
3. Run `AnyPS5.exe`, select a valid x86-64 ELF or unencrypted/uncompressed SELF (`eboot.bin`), choose an output `.exe`, and click **Convert**. The GUI automatically reconstructs SELF inputs and modules inside adjacent `sce_module`, `sce_modules`, and `prx` folders in a temporary workspace. Their paths and names are preserved. Original files are never modified; temporary copies are deleted after conversion. Encrypted/compressed, invalid, or unsupported SELF inputs report a clear error naming the offending file.
   After a successful conversion, the GUI copies bundled `.prx` and `.dll` runtime files into a `libs` folder beside the chosen output, without overwriting existing files. If libraries are absent or copying fails, the GUI displays a warning; the translated executable alone is not sufficient to run a game.
4. Read the relinker output in the GUI. Successful conversion does not guarantee a game can start or run correctly.

The GUI forwards upstream's `--windows` plus selected supported options: `--windows-gui`, `--windows-diagnostics`, `--to-intel`, `--registry`, `--lazy-binding`, `--skip-syscall-check`, `--skip-sce-module`, and `--autorun`. Leave advanced skips off unless debugging a specific title. The obsolete fork-only compatibility-report option is intentionally omitted.

SELF extraction requires plain, uncompressed ELF segments and does not bypass encryption or provide decryption keys. Modules outside the adjacent `sce_module`, `sce_modules`, or `prx` trees are not staged. Extracting a module does not guarantee it is otherwise supported by the relinker.

This feature makes no changes to upstream GPU, audio, input, PRX emulation, or executable translation behavior. Runtime support and game compatibility remain limited. Use legally obtained, appropriately decrypted executables.

## Building
On Windows with a C++20-compatible MinGW toolchain:
```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --target relinker anyps5_gui --parallel 4
cmake --build build --target libs --parallel 4
```
For tests, run `ctest --test-dir build --output-on-failure`. The GUI is a thin launcher; automated game compatibility is not implied by a successful build.

The original pre-upstream-realignment implementation is archived in `backup/main-pre-upstream-20261010`.
