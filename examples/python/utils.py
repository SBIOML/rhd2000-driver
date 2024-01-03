def read_cffi_h_to_str(file):
    text = []
    with open(file, "r") as f:
        text = f.read().split("// CFFI START")[1].split("// CFFI END")[0]
    return text
