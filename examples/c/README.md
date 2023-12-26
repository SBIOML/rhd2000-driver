# RHD2000 C example

## General description

This short example shows how to use the RHD2000 lib in your pure C projects. Only a few things are necessary:

1. Include the library
2. Create a `rhd_device_t` object, referred as `dev` from now
3. Write a custom `rw` function to link to `dev` in the next step. It must follow the API given by `rhd_rw_t` in `rhd.h`, but its implementation and capabilities are fully up to the user
4. Initialize `dev` with `rhd_init`. This notably links the previously defined `rhd_rw_t` function to the structure and sets the bits doubling mode for RHD2164
5. (optional) Use `rhd_setup` to initialize RHD2164 with sensible defaults for EMG signal sampling at 1kHz
6. Use the driver's functions as you please

## Running

To compile and run the script, feel free to use `run.sh` located in this directory.
