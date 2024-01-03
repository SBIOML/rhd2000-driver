from cffi import FFI


def build():
    """
    This function shows how to build librhd with CFFI.
    """

    from utils import read_cffi_h_to_str

    ffibuilder = FFI()

    text = read_cffi_h_to_str("src/rhd.h")
    text2 = read_cffi_h_to_str("examples/python/cffi_rw.h")

    # cdef() expects a single string declaring the C types, functions and
    # globals needed to use the shared object. It must be in valid C syntax.
    ffibuilder.cdef(text)
    ffibuilder.cdef(text2)

    # set_source() gives the name of the python extension module to
    # produce, and some C source code as a string.  This C code needs
    # to make the declarated functions, types and globals available,
    # so it is often just the "#include".
    ffibuilder.set_source(
        "_rhd_cffi",
        """
        #include "../../src/rhd.h"
        #include "cffi_rw.h"
        """,
        sources=["../../src/rhd.c", "cffi_rw.c"],
    )

    return ffibuilder


def test():
    from _rhd_cffi import ffi, lib
    import time
    from timeit import default_timer as timer

    print(dir(lib))

    dev = ffi.new("rhd_device_t*")

    # for function pointer: https://stackoverflow.com/a/30811087/12135442

    TEST = "echo"

    if TEST == "serial":
        # Test with a real serial port
        lib.setup_serial("/dev/ttyUSB0".encode("ascii"))
        lib.rhd_init(dev, True, ffi.addressof(lib, "my_rhd_rw_serial"))
    else:
        # Test echoing back to console
        lib.rhd_init(dev, True, ffi.addressof(lib, "my_rhd_rw"))

    lib.rhd_setup(dev)
    lib.rhd_r(dev, lib.INTAN_0, 5)

    time.sleep(0.5)

    start = timer()
    lib.rhd_sample_all(dev)
    end = timer()
    print("-----------")
    print(f"Time to sample all : {(end - start)*1000:.3f} ms")

    lib.close_serial()


if __name__ == "__main__":
    BUILD = True
    try:
        if BUILD:
            ffibuilder = build()
            ffibuilder.compile(tmpdir="examples/python/", verbose=True)

        test()
    except FileNotFoundError:
        print("Make sure to run this script from the repo's root !")
