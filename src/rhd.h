/** @file rhd.h
 *
 * @brief RHD C Driver header file
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2023 SBIOML. All rights reserved.
 */

#ifndef RHD2000_H
#define RHD2000_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Read Write function typedef. It is the interface to the RHD2xxx chip.
 *
 * When called, it must send out `tx_buf` while reading into `rx_buf`.
 *
 * @param tx_buf write buffer
 * @param rx_buf receive buffer
 * @param len number of 16-bits words to transfer.
 *
 * @returns int : Return code
 */
typedef int (*rhd_rw_t)(uint16_t *tx_buf, uint16_t *rx_buf, size_t len);

typedef struct
{
  rhd_rw_t rw;
  bool double_bits;
} rhd_device_t;

typedef union
{
  struct
  {
    uint16_t _ : 8;
    uint16_t value : 6;
    uint16_t command : 2;
  };
  uint16_t raw; // Raw 16-bit access
} rhd_packet_t;

typedef union
{
  struct
  {
    uint16_t value : 8;
    uint16_t reg : 6;
    uint16_t command : 2;
  };
  uint16_t raw; // Raw 16-bit access
} rhd_packet_t;

/**
 * @brief Register 0 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t adc_comp_select : 2;
  uint8_t adc_comp_bias : 2;
  uint8_t adc_vref_en : 1;
  uint8_t amp_fast_settle : 1;
  uint8_t adc_ref_bw : 2;
} rhd_adc_cfg_t;

/**
 * @brief Register 1 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t adc_buffer_bias : 6;
  uint8_t vdd_sense_en : 1;
} rhd_supply_sensor_adc_buf_bias_t;

/**
 * @brief Register 2 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t mux_bias : 6;
} rhd_mux_bias_t;

/**
 * @brief Register 3 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t digout : 1;
  uint8_t digout_hiz : 1;
  uint8_t temp_en : 1;
  uint8_t temp_s1 : 1;
  uint8_t temp_s2 : 1;
  uint8_t mux_load : 3;
} rhd_mux_load_temp_sensor_aux_dig_out_t;

/**
 * @brief Register 4 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t dsp_cutoff_f : 4;
  uint8_t dsp_en : 1;
  uint8_t abs_mode : 1;
  uint8_t twos_comp : 1;
  uint8_t weak_miso : 1;
} rhd_adc_out_t;

/**
 * @brief Register 5 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t zcheck_en : 1;
  uint8_t zcheck_sel_pol : 1;
  uint8_t zcheck_conn_all : 1;
  uint8_t zcheck_scale : 2;
  uint8_t zcheck_load : 1;
  uint8_t zcheck_dac_power : 1;
} rhd_imp_check_ctrl_t;

/**
 * @brief Register 6 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t zcheck_dac : 8;
} rhd_imp_check_dac_t;

/**
 * @brief Register 7 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t zcheck_sel : 6;
} rhd_imp_check_amp_sel_t;

/**
 * @brief Register 8 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rh1_dac1 : 6;
  uint8_t reserved : 1;
  uint8_t offchip_rh1 : 1;
} rhd_amp_bw_sel_0_t;

/**
 * @brief Register 9 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rh1_dac2 : 5;
  uint8_t reserved : 2;
  uint8_t adc_aux1_en : 1;
} rhd_amp_bw_sel_1_t;

/**
 * @brief Register 10 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rh2_dac1 : 6;
  uint8_t reserved : 1;
  uint8_t offchip_rh2 : 1;
} rhd_amp_bw_sel_2_t;

/**
 * @brief Register 11 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rh2_dac2 : 5;
  uint8_t reserved : 2;
  uint8_t adc_aux2_en : 1;
} rhd_amp_bw_sel_3_t;

/**
 * @brief Register 12 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rl_dac1 : 7;
  uint8_t offchip_rl : 1;
} rhd_amp_bw_sel_4_t;

/**
 * @brief Register 13 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t rl_dac2 : 6;
  uint8_t rl_dac3 : 1;
  uint8_t adc_aux3_en : 1;
} rhd_amp_bw_sel_4_t;

/**
 * @brief Register 14 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr0 : 1;
  uint8_t apwr1 : 1;
  uint8_t apwr2 : 1;
  uint8_t apwr3 : 1;
  uint8_t apwr4 : 1;
  uint8_t apwr5 : 1;
  uint8_t apwr6 : 1;
  uint8_t apwr7 : 1;
} rhd_ind_amp_pwr_0_t;

/**
 * @brief Register 15 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr8 : 1;
  uint8_t apwr9 : 1;
  uint8_t apwr10 : 1;
  uint8_t apwr11 : 1;
  uint8_t apwr12 : 1;
  uint8_t apwr13 : 1;
  uint8_t apwr14 : 1;
  uint8_t apwr15 : 1;
} rhd_ind_amp_pwr_1_t;

/**
 * @brief Register 16 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr16 : 1;
  uint8_t apwr17 : 1;
  uint8_t apwr18 : 1;
  uint8_t apwr19 : 1;
  uint8_t apwr20 : 1;
  uint8_t apwr21 : 1;
  uint8_t apwr22 : 1;
  uint8_t apwr23 : 1;
} rhd_ind_amp_pwr_2_t;

/**
 * @brief Register 17 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr24 : 1;
  uint8_t apwr25 : 1;
  uint8_t apwr26 : 1;
  uint8_t apwr27 : 1;
  uint8_t apwr28 : 1;
  uint8_t apwr29 : 1;
  uint8_t apwr30 : 1;
  uint8_t apwr31 : 1;
} rhd_ind_amp_pwr_3_t;

/**
 * @brief Register 18 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr32 : 1;
  uint8_t apwr33 : 1;
  uint8_t apwr34 : 1;
  uint8_t apwr35 : 1;
  uint8_t apwr36 : 1;
  uint8_t apwr37 : 1;
  uint8_t apwr38 : 1;
  uint8_t apwr39 : 1;
} rhd_ind_amp_pwr_4_t;

/**
 * @brief Register 19 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr40 : 1;
  uint8_t apwr41 : 1;
  uint8_t apwr42 : 1;
  uint8_t apwr43 : 1;
  uint8_t apwr44 : 1;
  uint8_t apwr45 : 1;
  uint8_t apwr46 : 1;
  uint8_t apwr47 : 1;
} rhd_ind_amp_pwr_5_t;

/**
 * @brief Register 20 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr48 : 1;
  uint8_t apwr49 : 1;
  uint8_t apwr50 : 1;
  uint8_t apwr51 : 1;
  uint8_t apwr52 : 1;
  uint8_t apwr53 : 1;
  uint8_t apwr54 : 1;
  uint8_t apwr55 : 1;
} rhd_ind_amp_pwr_6_t;

/**
 * @brief Register 21 configuration
 * Assumes little-endian platform.
 */
typedef struct
{
  uint8_t apwr56 : 1;
  uint8_t apwr57 : 1;
  uint8_t apwr58 : 1;
  uint8_t apwr59 : 1;
  uint8_t apwr60 : 1;
  uint8_t apwr61 : 1;
  uint8_t apwr62 : 1;
  uint8_t apwr63 : 1;
} rhd_ind_amp_pwr_7_t;

typedef union
{
  rhd_adc_cfg_t adc_cfg;
  rhd_supply_sensor_adc_buf_bias_t supply_sensor_adc_buf_bias;
  rhd_mux_bias_t mux_bias;
  rhd_mux_load_temp_sensor_aux_dig_out_t mux_load_temp_sensor_aux_dig_out;
  rhd_adc_out_t adc_out;
  rhd_imp_check_ctrl_t imp_check_ctrl;
  rhd_imp_check_dac_t imp_check_dac;
  rhd_imp_check_amp_sel_t imp_check_amp_sel;
  rhd_amp_bw_sel_0_t amp_bw_sel_0;
  rhd_amp_bw_sel_1_t amp_bw_sel_1;
  rhd_amp_bw_sel_2_t amp_bw_sel_2;
  rhd_amp_bw_sel_3_t amp_bw_sel_3;
  rhd_amp_bw_sel_4_t amp_bw_sel_4;
  rhd_amp_bw_sel_4_t amp_bw_sel_5;
  rhd_ind_amp_pwr_0_t ind_amp_pwr_0;
  rhd_ind_amp_pwr_1_t ind_amp_pwr_1;
  rhd_ind_amp_pwr_2_t ind_amp_pwr_2;
  rhd_ind_amp_pwr_3_t ind_amp_pwr_3;
  rhd_ind_amp_pwr_4_t ind_amp_pwr_4;
  rhd_ind_amp_pwr_5_t ind_amp_pwr_5;
  rhd_ind_amp_pwr_6_t ind_amp_pwr_6;
  rhd_ind_amp_pwr_7_t ind_amp_pwr_7;
  uint8_t raw;
} rhd_reg_cfg_t;

typedef enum
{
  ADC_CFG = 0,
  SUPPLY_SENSOR_ADC_BUF_BIAS = 1,
  MUX_BIAS_CURR = 2,
  MUX_LOAD_TEMP_SENS_AUX_DIG_OUT = 3,
  ADC_OUT_FMT_DPS_OFF_RMVL = 4,
  IMP_CHK_CTRL = 5,
  IMP_CHK_DAC = 6,
  IMP_CHK_AMP_SEL = 7,
  AMP_BW_SEL_0 = 8,
  AMP_BW_SEL_1 = 9,
  AMP_BW_SEL_2 = 10,
  AMP_BW_SEL_3 = 11,
  AMP_BW_SEL_4 = 12,
  AMP_BW_SEL_5 = 13,
  IND_AMP_PWR_0 = 14,
  IND_AMP_PWR_1 = 15,
  IND_AMP_PWR_2 = 16,
  IND_AMP_PWR_3 = 17,
  IND_AMP_PWR_4 = 18,
  IND_AMP_PWR_5 = 19,
  IND_AMP_PWR_6 = 20,
  IND_AMP_PWR_7 = 21,
  INTAN_0 = 40,
  INTAN_1 = 41,
  INTAN_2 = 42,
  INTAN_3 = 43,
  INTAN_4 = 44,
  MISO_A_B = 59,
  DIE_REV = 60,
  UNI_BIPLR_AMPS = 61,
  NB_AMP = 62,
  CHIP_ID = 63,
} rhd_reg_t;

/**
 * @brief Send data. Unlike `rhd_r` and `rhd_w`, this function does not set bits
 * [7:6] of reg. It does double the bits of `reg` and `val` if
 * `dev->double_bits` is true.
 *
 * @param dev pointer to rhd_device_t instance
 * @param data data to write
 * @return int received value
 */
int rhd_send(rhd_device_t *dev, rhd_packet_t data);

/**
 * @brief Send raw data. This function, unlike `rhd_send`, does not double bits.
 * It should only be used for highly optimized situations where `val` is
 * pre-doubled, for example at compile-time.
 *
 * @param dev pointer to rhd_device_t instance
 * @param val value to send that is put in dev->tx_buf[0]
 * @return int SPI communication return code
 */
int rhd_send_raw(rhd_device_t *dev, uint16_t val);

/**
 * @brief Read RHD register
 *
 * dev->rx_buf contains the received values.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg register to read, member of rhd_reg_t enum
 * @return int Read value
 */
int rhd_r(rhd_device_t *dev, uint16_t reg);

/**
 * @brief Write RHD register
 *
 * dev->tx_buf's content is overwritten with the commands.
 *
 * dev->rx_buf contains the received values.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg Register to write to
 * @param val Value to write into register
 * @return int SPI return code
 */
int rhd_w(rhd_device_t *dev, uint16_t reg, uint16_t val);

/**
 * @brief Initialize RHD device driver. Afterwards, call `rhd_setup(...)` to
 * ready the device.
 *
 * @param dev pointer to rhd_device_t instance
 * @param mode true if using hardware flipflop strategy, false otherwise.
 * @param rw pointer to the read/write function.
 * This is how RHD2164 driver bridges to the hardware.
 * Refer to @ref rhd_rw_t's inline documentation.
 *
 * @return int sanity check result, 0 for success. See @ref rhd_sanity_check for more details.
 */
int rhd_init(rhd_device_t *dev, bool mode, rhd_rw_t rw);

/**
 * @brief Setup RHD device with sensible defaults, including device calibration.
 *
 * @param dev pointer to rhd_device_t instance
 * @param fs target sampling rate per channel [Hz]
 * @param fl amplifier lowpass frequency [Hz]
 * @param fh amplifier highpass frequency [Hz]
 * @param dsp enable dsp
 * @param fdsp high-pass DSP cutoff frequency [Hz]
 * @return int sanity check result, 0 for success. See @ref rhd_sanity_check for more details.
 */
int rhd_setup(rhd_device_t *dev, float fs, float fl, float fh, bool dsp,
              float fdsp);

/**
 * @brief Configure RHD sample rate
 *
 * @param dev pointer to rhd_device_t instance
 * @param fs sample rate in Hz
 * @param n_ch number of active channels (includes temperature, etc), for
 * RHD2164, halve the number : 64 ch -> n_ch = 32
 * @return int SPI communication return code
 */
int rhd_cfg_fs(rhd_device_t *dev, float fs, int n_ch);

/**
 * @brief Configure RHD enabled channels
 *
 * @param dev pointer to rhd_device_t instance
 * @param channels_l bitmask of the channels (0-31) to enable, eg 0xFFFFFFFF
 * enables 0-31, 0xFFFF enables ch 0-16
 * @param channels_h bitmask of the channels (32-63) to enable, only for RHD2164
 * WATCH OUT! channels_h registers is reversed : MSb is lower channel, LSb is
 * higher
 * @return int SPI communication return code
 */
int rhd_cfg_ch(rhd_device_t *dev, uint32_t channels_l, uint32_t channels_h);

/**
 * @brief Configure RHD on-chip amplifiers analog bandwidth, which is a bandpass
 * Butterworth filter
 *
 * TODO support off-chip registers
 * @param dev pointer to rhd_device_t instance
 * @param fl Lower cutoff frequency
 * @param fh higher cutoff frequency
 * @return total samples per seconds
 */
int rhd_cfg_amp_bw(rhd_device_t *dev, float fl, float fh);

/**
 * @brief Configure RHD onboard dsp
 *
 * @param dev pointer to rhd_device_t instance
 * @param twos_comp true for two's complement data, false for offset binary.
 * Only for biosignal channels, others are offset binary.
 * @param abs_mode true for onboard data rectification, false for full-wave
 * data. Same constraint as twos_comp
 * @param dsp true to enable onboard DSP offset removal with first-order HP IIR
 * filter
 * @param fdsp DSP cutoff frequency
 * @param fs channel sampling frequency
 * @return int SPI communication return code
 */
int rhd_cfg_dsp(rhd_device_t *dev, bool twos_comp, bool abs_mode, bool dsp,
                float fdsp, float fs);

/**
 * @brief "Force read" a register, sending the "read" command 3 times
 * to get the expected value in the RX buffer.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg register to read from
 * @return int value read from the register
 */
int rhd_read_force(rhd_device_t *dev, int reg);

/**
 * @brief Sample a single RHD channel.
 * dev->tx_buf's content is overwritten with the commands.
 * dev->sample_buf is written to at the channel's index.
 *
 * With the DDR strategy, 2 channels are sampled at once, for a total of 32 bits in 16 clock cycles.
 *
 * If `dev->double_bits` is `false`, MISO A is captured in rx[0] and MISO B in rx[1]
 * If `dev->double_bits` is `true`, MISO A and MISO B are intertwined since all received bits are doubled.
 *
 * @param dev pointer to rhd_device_t instance
 * @param ch channel number to sample (0-31)
 * @return int SPI return code
 */
int rhd_sample(rhd_device_t *dev, uint8_t ch);

/**
 * @brief Run RHD calibration routine
 *
 * @param dev pointer to rhd_device_t instance
 * @return int SPI return code
 */
int rhd_calib(rhd_device_t *dev);

/**
 * @brief Clear RHD calibration
 *
 * @param dev pointer to rhd_device_t instance
 * @return int SPI return code
 */
int rhd_clear_calib(rhd_device_t *dev);

/**
 * @brief Read the INTAN registers (40-44) to verify if the chip is working.
 *
 * @param dev pointer to rhd_device_t instance
 * @return int 0 for success. Otherwise, returns the first register which failed.
 */
int rhd_sanity_check(rhd_device_t *dev);

/**
 * @brief Sequentially sample all RHD channels.
 * The values are saved into dev->sample_buf.
 * Channel 0's LSb is set to 0, while all others are set to 1.
 *
 * @param dev pointer to rhd_device_t instance
 */
void rhd_sample_all(rhd_device_t *dev);

/**
 * @brief Decode rx'd bytes in dev->rx_buf to dev->sample_buf
 *
 * @return int the returned value from the rx buffer
 */
int rhd_get_val_from_rx(rhd_device_t *dev);

/**
 * @brief Decode rx'd bytes in dev->rx_buf to dev->sample_buf
 *
 * @param ch channel of the sample [0-63] in dev->rx_buf
 */
void rhd_get_samples_from_rx(rhd_device_t *dev, uint16_t ch);

/**
 * @brief Duplicate the bits of a value.
 * It is assumed to be an 8-bits value.
 * For example, 0b01010011 becomes 0b0011001100001111
 *
 * @param val 8-bit value to double every bit
 * @return int the 16-bit value with duplicate bits.
 */
int rhd_duplicate_bits(uint8_t val);

/**
 * @brief Unsplit SPI DDR flip-flopped data
 *
 * @param data source data as 0bxyxy xyxy xyxy xyxy
 * @param a destination 8-bit data as 0bxxxx xxxx
 * @param b destination 8-bit data as 0byyyy yyyy
 */
void rhd_unsplit_u16(uint16_t data, uint8_t *a, uint8_t *b);

#endif /* RHD_H */