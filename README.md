# RHD2000 C Driver

## Introduction

This repo provides a C Driver for the RHD2000 family of chips, located in `src/`. It completely wraps interaction with these devices, even RHD2164, which has a nonstandard DDR SPI interface. It supports flip-flopped as well as non-flip flopped operation.

Inline documentation is provided. Otherwise, take a look at the [examples](#examples) or to [EMaGer BLE Server](https://github.com/SBIOML/emager-psoc-ble-server/tree/main)'s `main.c` and `spi_psoc.c`.

## Tests

Tests are located under `tests/rhd_test.cpp` and can be ran with CMake either manually or by running `./run_cmake.sh`. They use [GTest](https://github.com/google/googletest) and [CMake](https://cmake.org/).

## Examples

A few examples are provided in the `examples/` directory. Each example has its own readme to explain what's happening.

## Setting up

- C compiler (GCC/Clang)
- CMake
- GTest

## References

- SPI [tutorial](https://www.analog.com/en/analog-dialogue/articles/introduction-to-spi-interface.html)
- RHD2164 [datasheet](https://intantech.com/files/Intan_RHD2164_datasheet.pdf)

## TODO

- Verify compatibility with RHD2216 and RHD2132
- Go from `.sh` to CMake
- Add CFFI API example
- Add schematics for RHD2164 DDR flip-flop
