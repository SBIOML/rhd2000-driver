gcc -shared -o build/librhd.so -fPIC src/rhd.c
gcc -shared -o build/libcffi_rw.so -fPIC examples/python/cffi_rw.c