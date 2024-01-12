/** @file rhd.c
 *
 * @brief Platform-agnostic RHD2000 driver. Mostly aimed at RHD2164.
 *
 * COPYRIGHT NOTICE: (c) 2023 SBIOML.  All rights reserved.
 */

#include "rhd.h"

const uint16_t RHD_ADC_CH_CMD_DOUBLE[32] = {
    0x00,  0x03,  0x0C,  0x0F,  0x30,  0x33,  0x3C,  0x3F,  0xC0,  0xC3,  0xCC,
    0xCF,  0xF0,  0xF3,  0xFC,  0xFF,  0x300, 0x303, 0x30C, 0x30F, 0x330, 0x333,
    0x33C, 0x33F, 0x3C0, 0x3C3, 0x3CC, 0x3CF, 0x3F0, 0x3F3, 0x3FC, 0x3FF};
const uint16_t RHD_ADC_CH_CMD[32] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                     11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
                                     22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

int rhd_init(rhd_device_t *dev, bool mode, rhd_rw_t rw) {
  dev->double_bits = mode;
  dev->rw = rw;
  return 0;
}

int rhd_send_raw(rhd_device_t *dev, uint16_t val) {
  dev->tx_buf[0] = val;
  switch ((int)dev->double_bits) {
  case 0:
    return dev->rw(dev->tx_buf, dev->rx_buf, 1);
    break;
  default:
    return dev->rw(dev->tx_buf, dev->rx_buf, 2);
    break;
  }
}

int rhd_send(rhd_device_t *dev, uint16_t reg, uint16_t val) {
  switch ((int)dev->double_bits) {
  case 0: {
    dev->tx_buf[0] = (reg << 8) | (val & 0xFF);
    return dev->rw(dev->tx_buf, dev->rx_buf, 1);
    break;
  }
  default: {
    dev->tx_buf[0] = rhd_duplicate_bits(reg);
    dev->tx_buf[1] = rhd_duplicate_bits(val);
    return dev->rw(dev->tx_buf, dev->rx_buf, 2);
    break;
  }
  }
}

int rhd_r(rhd_device_t *dev, uint16_t reg, uint16_t val) {
  // reg is 6 bits, b[7,6] = [1, 1]
  reg = (reg & 0x3F) | 0xC0;
  return rhd_send(dev, reg, val);
}

int rhd_w(rhd_device_t *dev, uint16_t reg, uint16_t val) {
  // reg is 6 bits, b[7,6] = [1, 0]
  reg = (reg & 0x3F) | 0x80;
  return rhd_send(dev, reg, val);
}

int rhd_setup(rhd_device_t *dev) {
  // Write registers 0-21 sequentially
  // R0 : 1.225V Vref = 1, ADC comp bias = 3, ADC comp sel = 2
  // R4 : [b6] twoscomp = 1
  // High bandwidth (R8-R11) = 300 Hz
  // Low bandwifth (R12-R13) = 20 Hz
  uint16_t val_buf[] = {
      0b11011110, 0b00100000, 0b00101000, 0b00000010, 0b11000111, 0,
      0,          0,          6,          9,          2,          11,
      54,         0,          0xFF,       0xFF,       0xFF,       0xFF,
      0xFF,       0xFF,       0xFF,       0xFF};

  // dummy cmd
  rhd_r(dev, CHIP_ID, 0);
  rhd_r(dev, CHIP_ID, 0);

  int ret = 0;
  for (unsigned int i = 0; i < sizeof(val_buf) / sizeof(uint16_t); i++) {
    rhd_w(dev, i, val_buf[i]);
    if (i < 2) {
      continue;
    }
    int ch = i - 2;
    uint8_t val = rhd_get_val_from_rx(dev);
    if (val != val_buf[ch]) {
      ret = -1;
    }

    // printk("Register %d - Expected: 0x%x, Received: 0x%x\n", ch,
    // val_buf[ch],
    //        val);
  }

  return ret;
}

int rhd_calib(rhd_device_t *dev) {
  int ret = rhd_send(dev, 0b01010101, 0);

  for (int i = 0; i < 9; i++) {
    // 9 dummy cmds
    rhd_r(dev, CHIP_ID, 0);
  }

  return ret;
}

int rhd_clear_calib(rhd_device_t *dev) { return rhd_send(dev, 0b01101010, 0); }

int rhd_sample(rhd_device_t *dev, uint8_t ch) {
  int ret = rhd_send(dev, RHD_ADC_CH_CMD[ch], 0);
  rhd_get_samples_from_rx(dev, ch);
  return ret;
}

void rhd_sample_all(rhd_device_t *dev) {
  // Let ch0 sample from last iter, ask for ch1
  const uint16_t *RHD_ADC_CH =
      (int)dev->double_bits ? RHD_ADC_CH_CMD_DOUBLE : RHD_ADC_CH_CMD;

  rhd_send_raw(dev, RHD_ADC_CH[1]);

  // now we can loop til asked for ch32
  int ch = 0;
  for (int i = 2; i < 34; i++) {
    uint16_t cmd;
    if (i < 32) {
      cmd = RHD_ADC_CH[i];
    } else {
      cmd = RHD_ADC_CH[0];
    }
    // Value rx'd is CH[i-2]'s sample
    rhd_send_raw(dev, cmd);
    rhd_get_samples_from_rx(dev, ch++);
  }

  dev->sample_buf[1] &= 0xFE; // ch0 LSb set to 0 for alignment
}

void rhd_get_samples_from_rx(rhd_device_t *dev, uint16_t ch) {
  int ch_l = ch * 2;
  int ch_h = (ch + 32) * 2;

  switch ((int)dev->double_bits) {
  case 0: {
    dev->sample_buf[ch_l++] = (dev->rx_buf[0] >> 8) & 0xFF;
    dev->sample_buf[ch_l] = dev->rx_buf[0] & 0xFF;
    dev->sample_buf[ch_h++] = (dev->rx_buf[1] >> 8) & 0xFF;
    dev->sample_buf[ch_h] = dev->rx_buf[1] & 0xFF;
    break;
  }
  case 1: {
    rhd_unsplit_u16(dev->rx_buf[0], &dev->sample_buf[ch_l++],
                    &dev->sample_buf[ch_h++]);
    rhd_unsplit_u16(dev->rx_buf[1], &dev->sample_buf[ch_l],
                    &dev->sample_buf[ch_h]);
    break;
  }
  }
  dev->sample_buf[ch_l] = dev->sample_buf[ch_l] | 1;
  dev->sample_buf[ch_h] = dev->sample_buf[ch_h] | 1;
}

int rhd_get_val_from_rx(rhd_device_t *dev) {
  switch ((int)dev->double_bits) {
  case 0:
    return dev->rx_buf[0] & 0xFF;
    break;
  default: {
    uint8_t val, dum;
    rhd_unsplit_u16(dev->rx_buf[1], &val, &dum);
    return val;
    break;
  }
  }
}

int rhd_duplicate_bits(uint8_t val) {
  int out = 0;
  for (int i = 0; i < 8; i++) {
    int tmp = (val >> i) & 1;
    out |= (tmp << 1 | tmp) << 2 * i;
  }
  return out;
}

void rhd_unsplit_u16(uint16_t data, uint8_t *a, uint8_t *b) {
  static const uint8_t shift_arr[] = {0x1,  0x2,  0x4,  0x8,
                                      0x10, 0x20, 0x40, 0x80};

  uint8_t aa = 0;
  uint8_t bb = 0;

  uint16_t dataa = data >> 1;
  // Doubt we can optimize it much more
  for (int i = 0; i < 8; i++) {
    aa |= (dataa >> i) & shift_arr[i];
    bb |= (data >> i) & shift_arr[i];
  }
  *a = aa;
  *b = bb;
}
