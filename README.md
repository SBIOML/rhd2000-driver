# RHD2000 C Driver

## Introduction

A C Driver, located in `src/`, is provided. It completely wraps interaction with an RHD2000 family of chips, even RHD2164, which has a nonstandard DDR SPI interface. It supports flip-flopped as well as non-flip flopped operation.

Inline documentation is provided. Otherwise, take a look at [EMaGer BLE Server](https://github.com/SBIOML/emager-psoc-ble-server/tree/main)'s `main.c` and `spi_psoc.c` for example usage in Flip-Flop mode.

Tests are located under `tests/rhd_test.cpp` and can be ran with CMake either manually or by running `./run_cmake.sh`. They use [GTest](https://github.com/google/googletest) and [CMake](https://cmake.org/).

## Examples

A few examples are provided in the `examples` directory. Each example has its own readme to explain what's happening.

## Setting up

- CMake
- GTest

## References

- SPI [tutorial](https://www.analog.com/en/analog-dialogue/articles/introduction-to-spi-interface.html)
- RHD2164 [datasheet](https://intantech.com/files/Intan_RHD2164_datasheet.pdf)

## TODO

- Verify compatibility with RHD2216 and RHD2132
- Go from `.sh` to CMake
- Add CFFI API example
