#include <gtest/gtest.h>

extern "C" {
#include "rhd.h"
}

TEST(RHD, DupeUnsplit) {
  uint8_t a[] = {135, 42, 187, 91,  14,  239, 55,  178, 63, 105,
                 200, 33, 76,  162, 208, 4,   117, 88,  22, 195};
  for (int i = 0; i < sizeof(a); i++) {
    uint8_t ta;
    uint8_t tb;
    int ret = rhd_duplicate_bits(a[i]);
    rhd_unsplit_u16(ret, &ta, &tb);
    EXPECT_EQ(ta, a[i]);
  }
}

TEST(RHD, DuplicateBits) {
  int val[] = {0xAA, 0x55};
  int exp[] = {0xCCCC, 0x3333};
  for (int i = 0; i < sizeof(val) / sizeof(int); i++) {
    int ret = rhd_duplicate_bits(val[i]);
    EXPECT_EQ(ret, exp[i]);
  }
}

TEST(RHD, UnsplitMiso) {
  int val[] = {0xCCCC, 0x3333};
  int exp[] = {0xAA, 0x55};
  for (int i = 0; i < sizeof(val) / sizeof(int); i++) {
    uint8_t ret, dum;
    rhd_unsplit_u16(val[i], &ret, &dum);
    EXPECT_EQ(ret, exp[i]);
  }
}

/**
 * Do not change this function !
 */
int rw(uint16_t *tx_buf, uint16_t *rx_buf, size_t len) {
  rx_buf[0] = 0xAAAA;
  rx_buf[1] = 0x5555;
  return len;
}

TEST(RHD, RhdInit) {
  rhd_device_t dev;

  rhd_init(&dev, 0, rw);
  EXPECT_EQ(rhd_send(&dev, 0, 0), 1);

  rhd_init(&dev, 1, rw);
  EXPECT_EQ(rhd_send(&dev, 0, 0), 2);
}

TEST(RHD, RhdSendRaw) {
  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int len = rhd_send_raw(&dev, 0xAA);
  EXPECT_EQ(dev.tx_buf[0], 0xAA);
  EXPECT_EQ(len, 1);

  rhd_init(&dev, 1, rw);
  len = rhd_send_raw(&dev, 0xAA);
  EXPECT_EQ(dev.tx_buf[0], 0xAA);
  EXPECT_EQ(len, 2);
}

TEST(RHD, RhdSend) {
  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int len = rhd_send(&dev, 0xAA, 0x55);
  EXPECT_EQ(dev.tx_buf[0] & 0xFF00, (0xAA) << 8);
  EXPECT_EQ(dev.tx_buf[0] & 0xFF, 0x55);
  EXPECT_EQ(len, 1);

  rhd_init(&dev, 1, rw);
  len = rhd_send(&dev, 0xAA, 0x55);
  EXPECT_EQ(dev.tx_buf[0], 0xCCCC);
  EXPECT_EQ(dev.tx_buf[1], 0x3333);
  EXPECT_EQ(len, 2);
}

TEST(RHD, RhdRead) {
  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int len = rhd_r(&dev, 0x0F);
  EXPECT_EQ(dev.tx_buf[0] & 0xFF00, 0xCF00);
  EXPECT_EQ(len, 1);
  EXPECT_EQ(dev.rx_buf[0], 0xAAAA);
  EXPECT_EQ(dev.rx_buf[1], 0x5555);

  rhd_init(&dev, 1, rw);
  len = rhd_r(&dev, 0x0F);
  EXPECT_EQ(dev.tx_buf[0], 0xF0FF);
  EXPECT_EQ(len, 2);
  EXPECT_EQ(dev.rx_buf[0], 0xAAAA);
  EXPECT_EQ(dev.rx_buf[1], 0x5555);
}

TEST(RHD, RhdWrite) {
  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int len = rhd_w(&dev, 0x0F, 0x55);
  EXPECT_EQ(dev.tx_buf[0] & 0xFF00, 0x8F00);
  EXPECT_EQ(dev.tx_buf[0] & 0xFF, 0x55);
  EXPECT_EQ(len, 1);

  rhd_init(&dev, 1, rw);
  len = rhd_w(&dev, 0x0F, 0x55);
  EXPECT_EQ(dev.tx_buf[0], 0xC0FF);
  EXPECT_EQ(dev.tx_buf[1], 0x3333);
  EXPECT_EQ(len, 2);
}

TEST(RHD, RhdClearCalib) {
  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int len = rhd_clear_calib(&dev);
  EXPECT_EQ(dev.tx_buf[0], 0b01101010 << 8);
  EXPECT_EQ(len, 1);

  rhd_init(&dev, 1, rw);
  len = rhd_clear_calib(&dev);
  EXPECT_EQ(dev.tx_buf[0], 0b0011110011001100);
  EXPECT_EQ(len, 2);
}

TEST(RHD, RhdSample) {
  extern int RHD_CHANNEL_MAP[32];

  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  int ch = 10;

  int len = rhd_sample(&dev, ch);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[ch]], 0xAAAA | 1);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[ch + 32]], 0x5555);
  EXPECT_EQ(len, 1);

  rhd_init(&dev, 1, rw);
  len = rhd_sample(&dev, ch);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[ch]], 0xFF00 | 1);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[ch + 32]], 0x00FF | 1);
  EXPECT_EQ(len, 2);
}

TEST(RHD, RhdSampleAll) {
  extern const uint16_t RHD_ADC_CH_CMD[32];
  extern const uint16_t RHD_ADC_CH_CMD_DOUBLE[32];
  extern int RHD_CHANNEL_MAP[32];

  rhd_device_t dev;
  rhd_init(&dev, 0, rw);
  rhd_sample_all(&dev);
  EXPECT_EQ(dev.tx_buf[0], RHD_ADC_CH_CMD[0]);
  EXPECT_EQ(dev.sample_buf[0] & 1, 0); // Check channel 0 lsb == 0
  EXPECT_EQ(dev.sample_buf[1] & 1, 1); // Check other channel lsb

  // Check all channel values
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[0]] & 0xFFFE, 0xAAAA & 0xFFFE);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[32]] & 0xFFFE, 0x5555 & 0xFFFE);

  for (int i = 1; i < 32; i++) {
    EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[i]] & 0xFFFE, 0xAAAA);
    EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[i + 32]] & 0xFFFE,
              0x5555 & 0xFFFE);
  }

  return;

  rhd_init(&dev, 1, rw);
  rhd_sample_all(&dev);
  EXPECT_EQ(dev.tx_buf[0], RHD_ADC_CH_CMD_DOUBLE[0]);
  EXPECT_EQ(dev.sample_buf[0] & 1, 0);
  EXPECT_EQ(dev.sample_buf[1] & 1, 1);

  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[0]], 0xFF00);
  EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[32]], 0x00FF);

  for (int i = 1; i < 32; i++) {
    EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[i]] & 0xFFFE, 0xFF00);
    EXPECT_EQ(dev.sample_buf[RHD_CHANNEL_MAP[i + 32]] & 0xFFFE, 0x00FE);
  }
}