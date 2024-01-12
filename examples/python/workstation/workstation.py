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
        os.path.dirname(__file__) + "/cffi_rw.h",
        os.path.dirname(__file__) + "/cffi_rw.c",
        os.path.dirname(__file__),
    )

    from _rhd_cffi import ffi, lib


def test_echo():
    print(dir(lib))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    # Test echoing back to console
    lib.rhd_init(dev, True, ffi.addressof(lib, "my_rhd_rw"))
    lib.rhd_setup(dev)
    lib.rhd_r(dev, lib.INTAN_0, 5)

    cffi_utils.benchmark(lib.rhd_sample_all, 10, dev)


def test_serial():
    print(dir(lib))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    # Test with a real serial port
    lib.setup_serial("/dev/ttyUSB0".encode("ascii"))
    lib.rhd_init(dev, True, ffi.addressof(lib, "my_rhd_rw_serial"))
    lib.rhd_setup(dev)
    lib.rhd_r(dev, lib.INTAN_0, 5)

    cffi_utils.benchmark(lib.rhd_sample_all, 10, dev)

    lib.close_serial()


if __name__ == "__main__":
    test_echo()
    # test_serial()
