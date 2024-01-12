import sys
import os

sys.path.append(os.path.dirname(__file__) + "/../")  # patch PATHs

import cffi_utils

try:
    # Try to import CFFI
    from _rhd_cffi import ffi, lib
except:
    # Build it if import failed

    cffi_utils.build_cffi(
        "src/rhd.h",
        "src/rhd.c",
        os.path.dirname(__file__) + "/rhd_pynq.h",
        os.path.dirname(__file__) + "/rhd_pynq.c",
        os.path.dirname(__file__),
    )

    from _rhd_cffi import ffi, lib


def test():
    print(dir(lib))

    print(lib.rhd_pynq_setup())

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    lib.rhd_init(dev, False, ffi.addressof(lib, "rhd_pynq_rw"))
    lib.rhd_setup(dev)

    for i in range(40, 45):
        for k in range(3):
            lib.rhd_r(dev, i, 0)

        print(f"Cmd read channel {i}, ret={chr(dev.rx_buf[0])}, {chr(dev.rx_buf[1])}")

    cffi_utils.benchmark(lib.rhd_r, 10000, dev, 40, 0)

    cffi_utils.benchmark(lib.rhd_sample_all, 1000, dev)

    print(lib.rhd_pynq_close())


if __name__ == "__main__":
    test()
