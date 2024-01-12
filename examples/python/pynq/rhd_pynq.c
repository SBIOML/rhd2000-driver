#include "rhd_pynq.h"
#include <pynq_api.h>
#include <stdint.h>
#include <stdio.h>

// AXI gpio 0: 0x4120 | 1 channel | 16 bits | spi_din
// AXI gpio 1: 0x4121 | 2 channels | 1 bit | spi_start, spi_done
// AXI gpio 2: 0x4122 | 2 channels | 16 bit | spi_dout_a, spi_dout_b
PYNQ_MMIO_WINDOW axi_gpio_0;
PYNQ_MMIO_WINDOW axi_gpio_1;
PYNQ_MMIO_WINDOW axi_gpio_2;

uint8_t *spi_done = NULL;
uint8_t *spi_start = NULL;

int rhd_pynq_setup() {

  if (spi_start != NULL) {
    return -1;
  }

  if (PYNQ_loadBitstream("rhd-spi.bit") != PYNQ_SUCCESS) {
    return 0;
  }

  PYNQ_createMMIOWindow(&axi_gpio_0, 0x41200000, 0x200);
  PYNQ_createMMIOWindow(&axi_gpio_1, 0x41210000, 0x200);
  PYNQ_createMMIOWindow(&axi_gpio_2, 0x41220000, 0x200);

  spi_start = (uint8_t *)malloc(sizeof(uint8_t));
  spi_done = (uint8_t *)malloc(sizeof(uint8_t));

  return PYNQ_SUCCESS;
}

int rhd_pynq_close() {
  PYNQ_closeMMIOWindow(&axi_gpio_0);
  PYNQ_closeMMIOWindow(&axi_gpio_1);
  PYNQ_closeMMIOWindow(&axi_gpio_2);

  free(spi_start);
  free(spi_done);

  return PYNQ_SUCCESS;
}

int rhd_pynq_rw(uint16_t *tx, uint16_t *rx, size_t len) {
  for (unsigned int i = 0; i < len; i++) {
    PYNQ_writeMMIO(&axi_gpio_0, &tx[i], 0, sizeof(uint16_t));

    *spi_start = 1;
    PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));
    *spi_start = 0;
    PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));

    do {
      PYNQ_readMMIO(&axi_gpio_1, spi_done, 8, sizeof(uint8_t));
    } while (*spi_done == 0);
    *spi_done = 0;

    PYNQ_readMMIO(&axi_gpio_2, &rx[0], 0, sizeof(uint16_t));
    PYNQ_readMMIO(&axi_gpio_2, &rx[1], 8, sizeof(uint16_t));
  }
  return len;
}

int rhd_pynq_find_ch2() {
  *spi_start = 1;
  PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));
  *spi_start = 0;
  PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));

  for (volatile int i = 0; i < 1000; i++) {
    ;
  }

  *spi_done = 0;
  for (int i = 0; i < 16; i++) {
    PYNQ_readMMIO(&axi_gpio_1, spi_done, i, sizeof(uint8_t));
    printf("%d: SPI_DONE=%d\n", i, *spi_done);
  }

  return *spi_done;
}

int main() {
  rhd_pynq_setup();

  uint16_t tx_buf[2] = {0};
  uint16_t rx_buf[2] = {0};

  for (int i = 0; i < 8; i++) {
    tx_buf[0] = 0xC000 | ((40 + i) << 8);
    rhd_pynq_rw(tx_buf, rx_buf, 1);
    if (i > 2) {
      printf("%c\n", rx_buf[0]);
    }
  }

  rhd_pynq_find_ch2();

  rhd_pynq_close();

  return 0;
}
