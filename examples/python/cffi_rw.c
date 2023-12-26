#include <stdio.h>

#include "cffi_rw.h"

int my_rhd_rw(uint16_t *tx_buf, uint16_t *rx_buf, size_t len) {
  printf("R/W %d words to SPI: ", (int)len);
  for (int i = 0; i < len; i++) {
    printf("0x%x ", tx_buf[i]);
  }
  printf("\n");
  return 1;
}