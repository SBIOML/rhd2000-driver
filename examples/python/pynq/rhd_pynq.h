#include "../../../src/rhd.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
// CFFI START

void *malloc(size_t size);
void free(void *ptr);

/**
 * @brief Setup the PYNQ C environment.
 *
 * @param bitstream path to bitstream, if empty, will not load it, eg for Python
 * CFFI uses
 * @param clk_div 2x clock divider (eg 5 divides SPI by 10 vs PL clock)
 * @param clk_wait clock cycles to wait after a transfer is finished
 * @return int error code
 */
int rhd_pynq_setup(char bitstream[], uint16_t clk_div, uint8_t clk_wait,
                   bool load);

/**
 * @brief Cleanup RHD PYNQ before exiting.
 *
 * @return int
 */
int rhd_pynq_close(void);

/**
 * @brief Custom DDR SPI read/write function.
 *
 * @param tx pointer to tx buffer
 * @param rx pointer to rx buffer, will only receive the last transfer in case
 * of multiples
 * @param len transfer length, tx and rx assumed to be long enough
 * @return int
 */
int rhd_pynq_rw(uint16_t *tx, uint16_t *rx, size_t len);

/**
 * @brief C sampling routine. Used to take advantage of C's immense performance
 * advantage versus Python.
 *
 * @param dev
 * @param nsamples
 * @param dt_micro
 * @return uint16_t*
 */
uint16_t *rhd_pynq_sampling(rhd_device_t *dev, uint32_t nsamples,
                            uint32_t dt_micro);

// CFFI END
