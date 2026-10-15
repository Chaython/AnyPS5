# About

Tool for automatic executables porting to Linux and Windows.

Includes a [relinker](core/relinker) that converts executable to the target system's native format and implementations of [system prx libraries](core/libs/prx) suitable for dynamic linking. No emulation or separate runtime process.

[Usage](docs/user/USAGE.md), [Build instructions](docs/dev/BUILD.md), [Architecture](docs/dev/ARCHITECTURE.md), [Technical debt of the project](docs/dev/TechnicalDebt.md), [code style conventions](docs/dev/CONVENTIONS.md), [contributing](CONTRIBUTING.md)

## Windows converter (fork)

This fork provides an optional Windows-native GUI (`AnyPS5.exe`) with adaptive Windows accent/dark/light themes and automatic preparation of supported unencrypted SELF executables and guest modules. It uses upstream's `relinker.exe` and does not replace the game runtime. See [Windows GUI usage](docs/user/WINDOWS_GUI.md).

## Status

[![libraries](https://boykopovar.github.io/AnyPS5/badge-libraries.svg)](https://boykopovar.github.io/AnyPS5/) [![shaders](https://boykopovar.github.io/AnyPS5/badge-shaders.svg)](https://boykopovar.github.io/AnyPS5/)

[![progress map](https://boykopovar.github.io/AnyPS5/progress.svg)](https://boykopovar.github.io/AnyPS5/)

<sub>* System libraries: percentage of the functions known to the project so far (declared in [core/libs/prx](core/libs/prx)), not of every PS5 system function. The total grows as more functions are declared.</sub>

[List of verified games](docs/user/COMPATIBILITY.md)

Dreaming Sarah (2D platformer) runs at a stable 60 fps on a GTX 1050 Ti / i5-7500 3.4GHz.

Unsupported or unexpected states strictly throw `std::runtime_error`. `what()` is printed to stderr and the process terminates.

The [shader recompiler](core/shader/recompiler/Recompiler.cpp) successfully produces SPIR-V (validated via [Spirv-Tools](3rdparty/SPIRV-Tools) when built with `ANYPS5_ENABLE_SPIRV_TOOLS`).

## Compatibility

See the [game compatibility list](docs/user/COMPATIBILITY.md) for tested games and known issues.

## Input mapping

SDL-mapped game controllers are supported, including analog sticks and triggers. Keyboard and mouse controls can be configured with an `anyps5-input.ini` file. See [input mapping](docs/user/INPUT_MAPPING.md) for the supported devices and configuration format.

## Disclaimer

This project is intended for interoperability, research, preservation, and compatibility purposes. It does not include, distribute, or require copyrighted software, firmware, cryptographic keys, or proprietary libraries. Users are responsible for ensuring that any binaries used with this project are obtained and used in accordance with applicable laws and their respective license terms.

## License

This project is licensed under the GNU General Public License version 2 only.

<!-- chaython-funding:start -->
## Support development

Support this project through [GitHub Sponsors](https://github.com/sponsors/Chaython), [Patreon](https://patreon.com/Chaython), [Buy Me a Coffee](https://buymeacoffee.com/chaython), or [PayPal](https://paypal.me/ChaythonM).

Crypto donations:

- **Ethereum (ETH):** `0x201B6E0B0282BBAE1592C24f9CbD39009A70F7d7`
- **Bitcoin (BTC):** `32o7JmXgFcs4R1SbcLLVKQCRw9KHkhneJi`
- **Solana (SOL):** `BDicyUL9zizDeYBLw8oBwotv6jtHotLLQD7Bcqg2atLk`
- **USDC (Base network):** `0xBeD35dcAd9F1A32459B3C0DF004371EBE939E760`

**Send USDC only on Base.**
<!-- chaython-funding:end -->
