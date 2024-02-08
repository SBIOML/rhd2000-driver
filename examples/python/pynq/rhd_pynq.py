import sys
import os

sys.path.append(os.path.dirname(__file__) + "/../")  # patch PATHs

import cffi_utils

def test():
    from _rhd_cffi import ffi, lib

    # print(dir(lib))

    print(lib.rhd_pynq_setup(b"examples/python/pynq/bitfile/design_1.bit", 5, 3))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    lib.rhd_init(dev, False, ffi.addressof(lib, "rhd_pynq_rw"))
    lib.rhd_setup(dev, 1000, 10, 500, True, 20)

    for i in range(10):
        p = lib.rhd_pynq_sampling(dev, 1000, 100)
        print([p[j] for j in range(10)])
        p = ffi.gc(p, lib.free)

    for i in range(40, 45):
        lib.rhd_read_force(dev, i)
        print(f"Cmd read channel {i}, ret={chr(dev.rx_buf[0])}, {chr(dev.rx_buf[1])}")

    cffi_utils.benchmark(lib.rhd_r, 10000, dev, 40)

    cffi_utils.benchmark(lib.rhd_sample_all, 1000, dev)

    print(lib.rhd_pynq_close())


if __name__ == "__main__":
    cffi_utils.build_cffi(
        "src/rhd.h",
        "src/rhd.c",
        os.path.dirname(__file__) + "/rhd_pynq.h",
        os.path.dirname(__file__) + "/rhd_pynq.c",
        ["pynq", "cma", "pthread"],
        os.path.dirname(__file__),
    )
    test()
