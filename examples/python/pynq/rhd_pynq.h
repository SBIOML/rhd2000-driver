
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// CFFI START

int rhd_pynq_setup(void);

int rhd_pynq_close(void);

int rhd_pynq_rw(uint16_t* tx, uint16_t* rx, size_t len);

int rhd_pynq_find_ch2();

// CFFI END
