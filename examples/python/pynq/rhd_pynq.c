#include "rhd_pynq.h"
#include <pynq_api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// AXI gpio 0: 0x4120 | 1 channel | 24 bits | control signals
// AXI gpio 1: 0x4121 | 2 channels | 1 bit | spi_start, spi_done
// AXI gpio 2: 0x4122 | 2 channels | 16/32 bit | spi_din / spi_dout_a,
// spi_dout_b
PYNQ_MMIO_WINDOW axi_gpio_0;
PYNQ_MMIO_WINDOW axi_gpio_1;
PYNQ_MMIO_WINDOW axi_gpio_2;

uint8_t *spi_done = NULL;
uint8_t *spi_start = NULL;

int rhd_pynq_setup(char bitstream[], uint16_t clk_div, uint8_t clk_wait) {
  if (spi_start != NULL) {
    return -1;
  }

  if (strlen(bitstream) > 0) {
    int ret = PYNQ_loadBitstream(bitstream);
    if (ret != PYNQ_SUCCESS) {
      printf("Error loading bitstream %d\n", ret);
      return ret;
    }
  } else {
    printf("Assuming bitstream is already loaded...\n");
  }

  PYNQ_createMMIOWindow(&axi_gpio_0, 0x41200000, 0x200);
  PYNQ_createMMIOWindow(&axi_gpio_1, 0x41210000, 0x200);
  PYNQ_createMMIOWindow(&axi_gpio_2, 0x41220000, 0x200);

  spi_start = (uint8_t *)malloc(sizeof(uint8_t));
  spi_done = (uint8_t *)malloc(sizeof(uint8_t));

  uint32_t cfg_val = clk_wait << 16 | clk_div;
  PYNQ_writeMMIO(&axi_gpio_0, &cfg_val, 0, 3);

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
    PYNQ_writeMMIO(&axi_gpio_2, &tx[i], 0, sizeof(uint16_t));

    *spi_start = 1;
    PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));
    *spi_start = 0;
    PYNQ_writeMMIO(&axi_gpio_1, spi_start, 0, sizeof(uint8_t));

    do {
      PYNQ_readMMIO(&axi_gpio_1, spi_done, 8, sizeof(uint8_t));
    } while (*spi_done == 0);
    *spi_done = 0;

    PYNQ_readMMIO(&axi_gpio_2, (uint32_t *)rx, 8, sizeof(uint32_t));
  }
  return len;
}

uint32_t get_timestamp_us() {
  // https://stackoverflow.com/a/5833240
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (uint32_t)1000000 + tv.tv_usec;
}

uint16_t *rhd_pynq_sampling(rhd_device_t *dev, uint32_t nsamples,
                            uint32_t dt_micro) {
  uint16_t *bigbuf = (uint16_t *)malloc(64 * nsamples * sizeof(uint16_t));
  for (uint32_t i = 0; i < nsamples; i++) {
    uint32_t t0 = get_timestamp_us();
    rhd_sample_all(dev);
    memcpy(bigbuf + (i * 64), dev->sample_buf, 64 * sizeof(uint16_t));
    while (get_timestamp_us() - t0 < dt_micro) {
      ;
    }
  }
  return bigbuf;
}
