#include "../../src/rhd.h"
#include <stdio.h>

int my_rhd_rw(uint16_t *tx_buf, uint16_t *rx_buf, size_t len) {
  printf("R/W %d words to SPI: ", (int)len);
  for (int i = 0; i < len; i++) {
    printf("0x%x ", tx_buf[i]);
  }
  printf("\n");
  return 1;
}

int main() {
  rhd_device_t rhd;

  rhd_init(&rhd, false, my_rhd_rw);

  rhd_setup(&rhd);

  rhd_r(&rhd, INTAN_0, 5);

  return 0;
}