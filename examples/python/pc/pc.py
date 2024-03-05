import sys
import os

sys.path.append(os.path.dirname(__file__) + "/../")  # patch PATHs

import cffi_utils


def test_echo():
    from _rhd_cffi import ffi, lib

    print(dir(lib))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    # Test echoing back to console
    lib.rhd_init(dev, False, ffi.addressof(lib, "my_rhd_rw"))
    lib.rhd_setup(dev, 1000, 20, 300, False, 1.25)
    lib.rhd_read_force(dev, lib.INTAN_0)

    # cffi_utils.benchmark(lib.rhd_sample_all, 10, dev)


def test_serial():
    from _rhd_cffi import ffi, lib

    print(dir(lib))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    # Test with a real serial port
    lib.setup_serial("/dev/ttyUSB0".encode("ascii"))
    lib.rhd_init(dev, True, ffi.addressof(lib, "my_rhd_rw_serial"))
    lib.rhd_setup(dev, 1000, 20, 500, True, 20)
    lib.rhd_r(dev, lib.INTAN_0, 5)

    cffi_utils.benchmark(lib.rhd_sample_all, 10, dev)

    lib.close_serial()


if __name__ == "__main__":
    cwdir = os.path.dirname(__file__)
    print(cwdir)

    cffi_utils.build_cffi(
        "src/rhd.h",
        "src/rhd.c",
        cwdir + "/cffi_rw.h",
        cwdir + "/cffi_rw.c",
        [],
        cwdir,
    )

    test_echo()
    # test_serial()
