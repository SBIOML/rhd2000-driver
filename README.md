# RHD2000 C Driver

## Introduction

This repo provides a C Driver for the RHD2000 family of chips, located in `src/`. It completely wraps interaction with these devices, even RHD2164, which has a nonstandard DDR SPI interface. It supports flip-flopped as well as non-flip flopped operation.

Inline documentation is provided. Otherwise, take a look at the [examples](#examples) or to [EMaGer BLE Server](https://github.com/SBIOML/emager-psoc-ble-server/tree/main)'s `main.c` and `spi_psoc.c`.

## Installation

**Installation only works on Linux**. You can install `librhd` as a system-wide shared library with the following commands from the repo's root:

```bash
make
sudo make install
sudo ldconfig
```

Then, as is shown in `examples/c/hello.c`, you can include `librhd` with `#include "rhd.h"`. Don't forget to _link_ `librhd`. For example, to compile `hello.c` with `gcc`: `gcc examples/c/hello.c -o build/hello_c_rhd -lrhd`.

## Uninstalling

You can uninstall `librhd` at any time from your system with:

```bash
sudo make uninstall
sudo ldconfig
```

It will simply delete the `librhd.{a, so}`and `rhd.h` from `/usr/local/lib/` and `/usr/local/include/`, respectively. Then, update the linker index with `ldconfig`.

## Tests

Tests are located under `tests/rhd_test.cpp`. They use [GTest](https://github.com/google/googletest) and [CMake](https://cmake.org/).

To run them, `make test`

## Examples

A few examples are provided in the `examples/` directory. Each example has its own readme to explain what's happening.

In short, if you want to use the library in a pure-C environment, check out `examples/c`. If, instead, you mainly use Python and still want to use librhd, look at `examples/python`.

## Setting up

- C compiler (GCC/Clang)
- CMake
- GTest

## References

- SPI [tutorial](https://www.analog.com/en/analog-dialogue/articles/introduction-to-spi-interface.html)
- RHD2000 [main page](https://intantech.com/products_RHD2000.html)
- RHD2000 [datasheet](https://intantech.com/files/Intan_RHD2000_series_datasheet.pdf)
- RHD2164 [datasheet](https://intantech.com/files/Intan_RHD2164_datasheet.pdf)
- Why install to `/usr/local/`? [Link](https://man7.org/conf/lca2006/shared_libraries/slide7.html)

## TODO

- Verify compatibility with RHD2216 and RHD2132
- Add schematics for RHD2164 DDR flip-flop
- Make sample_all use C=63 for auto incrementation (RHD2000 datasheet p.16)
