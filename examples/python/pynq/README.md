# PYNQ RHD Driver

This examples _needs_ to be ran from a [_PYNQ_ platform](http://www.pynq.io/). It also requires an AXI GPIO-abled bitstream that instanciates the custom RHD SPI IP.

This example requires PYNQ API library installed on the platform. Follow [this link](https://github.com/mesham/pynq_api/tree/master) to set it up.

The C code is responsible for:

- Loading the bitstream
- Accessing AXI GPIO peripherals (via `/dev/mem`) for SPI transfers
- Managing librhd
