from cffi import FFI
import shutil
import os
import time


def read_cffi_h_to_str(file: str) -> str:
    """
    This utility function allows to automate the process of CFFI's `cdef`.

    The file must contain 2 markers, which should be placed after `#includes`:
        - `CFFI START` to indicate start of declarations
        - `CFFI END` to indicate end of declarations

    Params:
        - file : path to the file to parse

    Returns the file path.

    ### Example
    >>> h_text = read_cffi_h_to_str(src/rhd.h)
    >>> ffibuilder.cdef(h_text)
    """

    text = ""
    with open(file, "r") as f:
        text = f.read().split("CFFI START")[1].split("CFFI END")[0]
    return text


def build_cffi(
    rhd_h_path: str,
    rhd_c_path: str,
    h_files: str,
    c_files: str,
    libraries=[],
    out_path="./",
):
    """
    This function takes care of the building phase of CFFI Out-of-line API mode.

    All paths must be relative to `PWD`, or absolute.

    It requires the paths to `librhd` sources (.h, .c).
    `args` allows to pass in an arbitrary list of other paths, which will be included in the CFFI build.
    For example, it's recommended to pass in your custom `rw` functor.
    """

    ffibuilder = FFI()
    ffibuilder.cdef(read_cffi_h_to_str(rhd_h_path))

    c_src = ""
    sources = []

    if type(h_files) == str:
        text = read_cffi_h_to_str(h_files)
        ffibuilder.cdef(text)
        c_src = f"""
            #include "{rhd_h_path}"
            #include "{h_files}"
            """
        sources = [rhd_c_path, c_files]

    elif type(h_files) == list:
        h_files = []
        for s in h_files:
            text = read_cffi_h_to_str(s)
            ffibuilder.cdef(text)
        c_src = f"""
        #include "{rhd_h_path}"
        """
        + "".join([f"#include {h}\n" for h in h_files])
        sources=[rhd_c_path, *c_files]

    if len(libraries) == 0:
        ffibuilder.set_source(
            "_rhd_cffi",
            c_src,
            sources=sources,
        )
    else:
        ffibuilder.set_source(
            "_rhd_cffi",
            c_src,
            sources=sources,
            libraries=libraries,
        )

    ffibuilder.compile(verbose=1)

    for f in os.listdir("."):
        if not f.startswith("_rhd_cffi"):
            continue
        print(f"Moving {f} to {out_path}/{f}")
        try:
            os.remove(f"{out_path}/{f}")
        except:
            pass
        shutil.move(f, out_path)


def benchmark(fn, n=1, *args):
    start = time.perf_counter_ns()
    for i in range(n):
        fn(*args)
    end = time.perf_counter_ns()

    print("-----------")
    print(
        f"Average dt for {n} iterations of {fn.__name__} : {(end - start)/(1000000*n):.3f} ms"
    )


if __name__ == "__main__":
    print("For examples, look in this folder's subfolders !")
    exit()
