/** @file rhd.c
 *
 * @brief Platform-agnostic RHD2000 driver. Mostly aimed at RHD2164, but should
 * work on RHD2000.
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

/**
 * @brief This array is used during `rhd_get_samples_from_rx` to dispatch the
 * received samples to the correct "physical" layout. Feel free to modify the
 * map as your application requires.
 */
int RHD_CHANNEL_MAP[64] = {10, 22, 12, 24, 13, 26, 7,  28, 1,  30, 59, 32, 53,
                           34, 48, 36, 62, 16, 14, 21, 11, 27, 5,  33, 63, 39,
                           57, 45, 51, 44, 50, 40, 8,  18, 15, 19, 9,  25, 3,
                           31, 61, 37, 55, 43, 49, 46, 52, 38, 6,  20, 4,  17,
                           2,  23, 0,  29, 60, 35, 58, 41, 56, 47, 54, 42};

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

int rhd_r(rhd_device_t *dev, uint16_t reg) {
  // reg is 6 bits, b[7,6] = [1, 1]
  reg = (reg & 0x3F) | 0xC0;
  return rhd_send(dev, reg, 0);
}

int rhd_w(rhd_device_t *dev, uint16_t reg, uint16_t val) {
  // reg is 6 bits, b[7,6] = [1, 0]
  reg = (reg & 0x3F) | 0x80;
  return rhd_send(dev, reg, val);
}

int rhd_init(rhd_device_t *dev, bool mode, rhd_rw_t rw) {
  dev->double_bits = mode;
  dev->rw = rw;
  return 0;
}

int rhd_setup(rhd_device_t *dev, int fs, int fl, int fh, bool dsp, int fdsp) {
  // R0 : 1.225V Vref = 1, ADC comp bias = 3, ADC comp sel = 2
  // R4 : [b6] twoscomp = 1
  // High bandwidth (R8-R11) = 300 Hz
  // Low bandwifth (R12-R13) = 20 Hz

  // dummy cmds
  rhd_r(dev, CHIP_ID);
  rhd_r(dev, CHIP_ID);

  // configure everything
  int ret = rhd_w(dev, ADC_CFG, 0b11011110);
  rhd_w(dev, MUX_LOAD_TEMP_SENS_AUX_DIG_OUT, 0b00000000);
  // TODO fn to cfg temp/digout ^
  rhd_w(dev, IMP_CHK_CTRL, 0);
  rhd_w(dev, IMP_CHK_DAC, 0);
  rhd_w(dev, IMP_CHK_AMP_SEL, 0);
  rhd_cfg_ch(dev, 0xFFFFFFFF, 0xFFFFFFFF);
  rhd_cfg_fs(dev, fs, 32);
  rhd_cfg_amp_bw(dev, fl, fh);
  rhd_cfg_dsp(dev, true, false, dsp, fdsp, fs);

  return ret;
}

int rhd_cfg_ch(rhd_device_t *dev, uint32_t channels_l, uint32_t channels_h) {
  rhd_w(dev, IND_AMP_PWR_0, channels_l & 0xFF);
  rhd_w(dev, IND_AMP_PWR_1, (channels_l >> 8) & 0xFF);
  rhd_w(dev, IND_AMP_PWR_2, (channels_l >> 16) & 0xFF);
  rhd_w(dev, IND_AMP_PWR_3, (channels_l >> 24) & 0xFF);

  rhd_w(dev, IND_AMP_PWR_4, channels_h & 0xFF);
  rhd_w(dev, IND_AMP_PWR_5, (channels_h >> 8) & 0xFF);
  rhd_w(dev, IND_AMP_PWR_6, (channels_h >> 16) & 0xFF);
  return rhd_w(dev, IND_AMP_PWR_7, (channels_h >> 24) & 0xFF);
}

int rhd_cfg_fs(rhd_device_t *dev, int fs, int n_ch) {
  int msps = fs * n_ch;
  int adc_buf_bias = 32;
  int mux_bias = 40;
  if (msps >= 700000) {
    adc_buf_bias = 2;
    mux_bias = 4;
  } else if (msps >= 525000) {
    adc_buf_bias = 3;
    mux_bias = 7;
  } else if (msps >= 440000) {
    adc_buf_bias = 3;
    mux_bias = 16;
  } else if (msps >= 350000) {
    adc_buf_bias = 4;
    mux_bias = 18;
  } else if (msps >= 280000) {
    adc_buf_bias = 8;
    mux_bias = 26;
  } else if (msps >= 220000) {
    adc_buf_bias = 8;
    mux_bias = 32;
  } else if (msps >= 175000) {
    adc_buf_bias = 8;
    mux_bias = 40;
  } else if (msps >= 140000) {
    adc_buf_bias = 16;
    mux_bias = 40;
  }

  rhd_w(dev, SUPPLY_SENS_ADC_BUF_BIAS, adc_buf_bias);
  rhd_w(dev, MUX_BIAS_CURR, mux_bias);

  return msps;
}

int rhd_cfg_amp_bw(rhd_device_t *dev, int fl, int fh) {
  int rh1_dac1 = 38;
  int rh1_dac2 = 26;
  int rh2_dac1 = 5;
  int rh2_dac2 = 31;

  if (fh >= 20000) {
    rh1_dac1 = 8;
    rh1_dac2 = 0;
    rh2_dac1 = 4;
    rh2_dac2 = 0;
  } else if (fh >= 15000) {
    rh1_dac1 = 11;
    rh1_dac2 = 0;
    rh2_dac1 = 8;
    rh2_dac2 = 0;
  } else if (fh >= 10000) {
    rh1_dac1 = 17;
    rh1_dac2 = 0;
    rh2_dac1 = 16;
    rh2_dac2 = 0;
  } else if (fh >= 7500) {
    rh1_dac1 = 22;
    rh1_dac2 = 0;
    rh2_dac1 = 23;
    rh2_dac2 = 0;
  } else if (fh >= 5000) {
    rh1_dac1 = 33;
    rh1_dac2 = 0;
    rh2_dac1 = 37;
    rh2_dac2 = 0;
  } else if (fh >= 3000) {
    rh1_dac1 = 3;
    rh1_dac2 = 1;
    rh2_dac1 = 13;
    rh2_dac2 = 1;
  } else if (fh >= 2500) {
    rh1_dac1 = 13;
    rh1_dac2 = 1;
    rh2_dac1 = 25;
    rh2_dac2 = 1;
  } else if (fh >= 2000) {
    rh1_dac1 = 27;
    rh1_dac2 = 1;
    rh2_dac1 = 44;
    rh2_dac2 = 1;
  } else if (fh >= 1500) {
    rh1_dac1 = 1;
    rh1_dac2 = 2;
    rh2_dac1 = 33;
    rh2_dac2 = 2;
  } else if (fh >= 1000) {
    rh1_dac1 = 46;
    rh1_dac2 = 2;
    rh2_dac1 = 30;
    rh2_dac2 = 3;
  } else if (fh >= 750) {
    rh1_dac1 = 41;
    rh1_dac2 = 3;
    rh2_dac1 = 36;
    rh2_dac2 = 4;
  } else if (fh >= 500) {
    rh1_dac1 = 30;
    rh1_dac2 = 5;
    rh2_dac1 = 43;
    rh2_dac2 = 6;
  } else if (fh >= 300) {
    rh1_dac1 = 6;
    rh1_dac2 = 9;
    rh2_dac1 = 2;
    rh2_dac2 = 11;
  } else if (fh >= 250) {
    rh1_dac1 = 42;
    rh1_dac2 = 10;
    rh2_dac1 = 5;
    rh2_dac2 = 13;
  } else if (fh >= 200) {
    rh1_dac1 = 24;
    rh1_dac2 = 13;
    rh2_dac1 = 7;
    rh2_dac2 = 16;
  } else if (fh >= 150) {
    rh1_dac1 = 44;
    rh1_dac2 = 17;
    rh2_dac1 = 8;
    rh2_dac2 = 21;
  } else if (fh >= 100) {
    rh1_dac1 = 38;
    rh1_dac2 = 26;
    rh2_dac1 = 5;
    rh2_dac2 = 31;
  }

  int rl_dac1 = 38;
  int rl_dac2 = 0;
  int rl_dac3 = 0;

  if (fl >= 500) {
    rl_dac1 = 13;
  } else if (fl >= 300) {
    rl_dac1 = 15;
  } else if (fl >= 250) {
    rl_dac1 = 17;
  } else if (fl >= 200) {
    rl_dac1 = 18;
  } else if (fl >= 150) {
    rl_dac1 = 21;
  } else if (fl >= 100) {
    rl_dac1 = 25;
  } else if (fl >= 75) {
    rl_dac1 = 28;
  } else if (fl >= 50) {
    rl_dac1 = 34;
  } else if (fl >= 30) {
    rl_dac1 = 44;
  } else if (fl >= 25) {
    rl_dac1 = 48;
  } else if (fl >= 20) {
    rl_dac1 = 54;
  } else if (fl >= 15) {
    rl_dac1 = 62;
  } else if (fl >= 10) {
    rl_dac1 = 5;
    rl_dac2 = 1;
  } else if (fl >= 7.5) {
    rl_dac1 = 18;
    rl_dac2 = 1;
  } else if (fl >= 5) {
    rl_dac1 = 40;
    rl_dac2 = 1;
  } else if (fl >= 3) {
    rl_dac1 = 20;
    rl_dac2 = 2;
  } else if (fl >= 2.5) {
    rl_dac1 = 42;
    rl_dac2 = 2;
  } else if (fl >= 2) {
    rl_dac1 = 8;
    rl_dac2 = 3;
  } else if (fl >= 1.5) {
    rl_dac1 = 9;
    rl_dac2 = 4;
  } else if (fl >= 1) {
    rl_dac1 = 44;
    rl_dac2 = 6;
  } else if (fl >= 0.75) {
    rl_dac1 = 49;
    rl_dac2 = 9;
  } else if (fl >= 0.5) {
    rl_dac1 = 35;
    rl_dac2 = 17;
  } else if (fl >= 0.3) {
    rl_dac1 = 1;
    rl_dac2 = 40;
  } else if (fl >= 0.25) {
    rl_dac1 = 56;
    rl_dac2 = 54;
  } else if (fl >= 0.1) {
    rl_dac1 = 16;
    rl_dac2 = 60;
    rl_dac3 = 1;
  }

  rhd_w(dev, AMP_BW_SEL_0, rh1_dac1);
  rhd_w(dev, AMP_BW_SEL_1, rh1_dac2);
  rhd_w(dev, AMP_BW_SEL_2, rh2_dac1);
  rhd_w(dev, AMP_BW_SEL_3, rh2_dac2);
  rhd_w(dev, AMP_BW_SEL_4, rl_dac1);
  int ret = rhd_w(dev, AMP_BW_SEL_5, (rl_dac3 << 6) | rl_dac2);

  return ret;
}

int rhd_cfg_dsp(rhd_device_t *dev, bool twos_comp, bool abs_mode, bool dsp,
                int fdsp, int fs) {
  float k_lut[] = {0.1103,     0.04579,     0.02125,    0.01027,
                   0.005053,   0.002506,    0.001248,   0.0006229,
                   0.0003112,  0.0001555,   0.00007773, 0.00003886,
                   0.00001943, 0.000009714, 0.000004857};
  int dsp_val = 0;

  if (dsp) {
    float k = ((float)fdsp) / fs;
    for (unsigned int i = 0; i < sizeof(k_lut) / sizeof(float); i++) {
      dsp_val++;
      if (k > k_lut[i]) {
        if (i == 0) {
          break;
        }
        // k_lut descending order so it's between k_lut[i-1] and k_lut[i]
        float d1 = k_lut[i - 1] - k;
        float d2 = k - k_lut[i];

        if (d1 < d2) {
          // previous value is closer so choose it
          dsp_val--;
        }

        break;
      }
    }
    // Perfect differentiator
    if (fdsp == 0) {
      dsp_val = 0;
    }
  }

  return rhd_w(dev, ADC_OUT_FMT_DPS_OFF_RMVL,
               1 << 7 | ((int)twos_comp) << 6 | ((int)abs_mode) << 5 |
                   ((int)dsp) << 4 | dsp_val);
}

int rhd_calib(rhd_device_t *dev) {
  int ret = rhd_send(dev, 0b01010101, 0);

  for (int i = 0; i < 9; i++) {
    // 9 dummy cmds
    rhd_r(dev, CHIP_ID);
  }

  return ret;
}

int rhd_clear_calib(rhd_device_t *dev) { return rhd_send(dev, 0b01101010, 0); }

int rhd_read_force(rhd_device_t *dev, int reg) {
  for (int i = 0; i < 2; i++) {
    rhd_r(dev, reg);
  }
  return rhd_r(dev, reg);
}

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
  for (int i = 2; i < 34; i++) {
    uint16_t cmd;
    if (i < 32) {
      cmd = RHD_ADC_CH[i];
    } else {
      cmd = RHD_ADC_CH[0];
    }
    // Value rx'd is CH[i-2]'s sample
    rhd_send_raw(dev, cmd);
    rhd_get_samples_from_rx(dev, i - 2);
  }

  dev->sample_buf[0] &= 0xFFFE; // ch0 LSb set to 0 for alignment
}

void rhd_get_samples_from_rx(rhd_device_t *dev, uint16_t ch) {
  int ch_a = ch;
  int ch_b = (ch + 32);

  switch ((int)dev->double_bits) {
  case 0: {
    dev->sample_buf[RHD_CHANNEL_MAP[ch_a]] = dev->rx_buf[0] | 1;
    dev->sample_buf[RHD_CHANNEL_MAP[ch_b]] = dev->rx_buf[1] | 1;
    break;
  }
  default: {
    uint8_t dat_a[2] = {0};
    uint8_t dat_b[2] = {0};
    rhd_unsplit_u16(dev->rx_buf[0], &dat_a[1], &dat_b[1]);
    rhd_unsplit_u16(dev->rx_buf[1], &dat_a[0], &dat_b[0]);
    dev->sample_buf[RHD_CHANNEL_MAP[ch_a]] =
        (((uint16_t)dat_a[1]) << 8) | dat_a[0] | 1;
    dev->sample_buf[RHD_CHANNEL_MAP[ch_b]] =
        (((uint16_t)dat_b[1]) << 8) | dat_b[0] | 1;
    break;
  }
  }
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
