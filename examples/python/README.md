# RHD2000 Python example

## Description

This subfolder contains some utilities for _building_ the CFFI bindings (`cffi_utils.py`) in Out-of-line API Mode. Then, the examples are split into subfolders, eg `pynq/`, `workstation/`. They start by building the `.so`s with _CFFI_ if needed, then run a function that calls `librhd`.

## Running

To run, execute the scripts from the repo's root, eg `python3 examples/python/workstation/workstation.py`
