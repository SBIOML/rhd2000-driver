from cffi import FFI


def read_cffi_h_to_str(file):
    text = []
    with open(file, "r") as f:
        text = f.read().split("// CFFI START")[1].split("// CFFI END")[0]
    return text


ffi = FFI()

# cdef() expects a single string declaring the C types, functions and
# globals needed to use the shared object. It must be in valid C syntax.
ffi.cdef(read_cffi_h_to_str("src/rhd.h"))
ffi.cdef(read_cffi_h_to_str("examples/python/cffi_rw.h"))
librhd = ffi.dlopen("build/librhd.so")
librw = ffi.dlopen("build/libcffi_rw.so")

dev = ffi.new("rhd_device_t*")
librhd.rhd_init(dev, False, librw.my_rhd_rw)
librhd.rhd_setup(dev)
librhd.rhd_r(dev, librhd.INTAN_0, 5)

print("Done !")
