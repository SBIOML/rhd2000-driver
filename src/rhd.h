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
 * @brief RHD2164 Read Write function typedef.
 * When called, it must send out w_buf while reading into r_buf.
 * The driver uses @ref rhd_device_t's rx_buf and tx_buf.
 *
 * @param tx_buf write buffer
 * @param rx_buf receive buffer
 * @param len number of 16-bit values to transfer.
 *
 * @returns int : Return code
 */
typedef int (*rhd_rw_t)(uint16_t *tx_buf, uint16_t *rx_buf, size_t len);

#define ZCHECK_EN(x)         ((x) & 0x1) // bit 0
#define ZCHECK_SCALE(x)   (((x) & 0x3) << 3) // Bit 4-3
#define ZCHECK_DAC_POWER(x)  (((x) & 0x1) << 6) // bit 6
#define ZCHECK_SCALE_0_1pf 0 // 0.1 pF - @ 1kHz -> max current = 0.38nA
#define ZCHECK_SCALE_1pf 1 // 1 pF - @ 1kHz -> max current = 3.8nA
#define ZCHECK_SCALE_10pf 3 // 10 pF - @ 1kHz -> max current = 38nA

#define DEFAULT_SCALE ZCHECK_SCALE_1pf // 1 pF - @ 1kHz -> max current = 3.8nA

typedef struct {
  bool enable; // true if impedance check is enabled
  uint8_t scale; // 0-3
  uint8_t voltage; // 0-255
  uint8_t electrode_reg; // 0-63
} rhd_impedance_t;

typedef enum {
  RHD_WAVEFORM_ERR_NONE = 0,
  RHD_WAVEFORM_ERR_AMPLITUDE = 1,
  RHD_WAVEFORM_ERR_BELOW_ZERO = 2,
  RHD_WAVEFORM_ERR_ABOVE_MAX = 3,
} rhd_waveform_error_t;

typedef struct {
    float phase;
    float phase_increment;
    int offset;
    int amplitude;
    int wave_freq;
    int sample_per_cycle;
    int sample_count;
    int sample;
    int error;
} rhd_waveform_state_t;

typedef struct
{
  rhd_rw_t rw;
  bool double_bits;
  rhd_impedance_t impedance; // impedance settings
} rhd_device_t;

typedef enum
{
  ADC_CFG = 0,
  SUPPLY_SENS_ADC_BUF_BIAS = 1,
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
 * @brief Send data. Unlike rhd_r and rhd_w, this function does not set bits
 * [7:6] of reg. It does double the bits of `reg` and `val` if
 * `dev->double_bits` is true.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg register to read, member of rhd_reg_t enum
 * @param val value to send
 * @return received value
 */
uint8_t rhd_send(rhd_device_t *dev, uint16_t reg, uint16_t val);

/**
 * @brief Read RHD register.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg register to read, member of rhd_reg_t enum
 * @return register value
 */
uint8_t rhd_r(rhd_device_t *dev, uint16_t reg);

/**
 * @brief Write RHD register.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg Register to write to
 * @param val Value to write into register
 * @return received value
 */
uint8_t rhd_w(rhd_device_t *dev, uint16_t reg, uint16_t val);

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
 * @return int sanity check result, 0 for success. See `rhd_sanity_check` for more details.
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
 * @param impedance enable impedance
 *
 * @return int sanity check result, 0 for success. See @ref rhd_sanity_check for more details.
 */
int rhd_setup(rhd_device_t *dev, float fs, float fl, float fh, bool dsp,
              float fdsp, bool impedance);

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
 * @brief Configure RHD impedance
 *
 * @param dev pointer to rhd_device_t instance
 * @param impedance_on true to enable impedance, false to disable
 * @param scale impedance scale
 * @param voltage impedance voltage
 * @param electrode_reg impedance electrode register
 */
int rhd_cfg_impedance(rhd_device_t *dev, bool impedance_on, uint8_t scale, uint8_t voltage, uint8_t electrode_reg);

/**
 * @brief Disable RHD impedance
 *
 * @param dev pointer to rhd_device_t instance
 */
int rhd_disable_impedance(rhd_device_t *dev);

/**
 * @brief Enable RHD impedance
 *
 * @param dev pointer to rhd_device_t instance
 */
int rhd_enable_impedance(rhd_device_t *dev);

/**
 * @brief Update RHD impedance DAC DC voltage
 *
 * @param dev pointer to rhd_device_t instance
 * @param voltage impedance voltage in volts (0-255)=(0-1.225V)
 */
int rhd_update_impedance_dac_voltage(rhd_device_t *dev, uint8_t voltage);

/**
 * @brief Update RHD impedance amplifier selection
 *
 * @param dev pointer to rhd_device_t instance
 * @param electrode_reg impedance electrode register
 */
int rhd_update_impedance_electrode_sel(rhd_device_t *dev, uint8_t electrode_reg); 



/**
 * @brief Initialize the waveform state for generating a waveform.
 *
 * @param offset The DC offset to center the waveform around. [128 is the 0V offset] (0..255)=(-1.1225..1.225V)
 * @param peak_val The peak value of the waveform. Must be bigger than offset [amplitude = peak - offset] (0-255)=(0-1.225V)
 * @param wave_freq The frequency of the waveform in Hz.
 * @param sample_per_cycle The number of samples to generate per cycle of the waveform.
 * @return 0 for success, 1 for failure.
 */
int rhd_waveform_init(rhd_waveform_state_t *state, int offset, int peak_val, float wave_freq, int sample_per_cycle);

/**
 * @brief Update the waveform state and write the generated waveform to the DAC.
 *
 * @param dev Pointer to the rhd_device_t structure representing the device.
 * @param state Pointer to the rhd_waveform_state_t structure containing the waveform state.
 * @return 0 for success, 1 for failure.
 */
int rhd_waveform_update(rhd_device_t *dev, rhd_waveform_state_t *state);

/**
   * @brief Generates a waveform and writes it to the DAC. Used to generate a AC voltage on the impedance electrode.
   *
   * This function continuously generates a sine waveform based on the specified parameters.
   * The waveform is centered around the specified offset and has a peak value defined by
   * the peak_val parameter. The frequency of the waveform is determined by the freq parameter,
   * and the number of samples per cycle is specified by sample_per_cycle. A callback function
   * for introducing delays between samples is provided as delay_us_fn_callback.
   *
   * @param dev Pointer to the rhd_device_t structure representing the device.
   * @param offset The DC offset to center the waveform around. (0-255)=(0-1.225V)
   * @param peak_val The peak value of the waveform. Must be bigger tan offset [amplitude = peak - offset] (0-255)=(0-1.225V)
   * @param freq The frequency of the waveform in Hz.
   * @param sample_per_cycle The number of samples to generate per cycle of the waveform.
   * @param delay_us_fn_callback A function pointer to a callback that introduces a delay in microseconds.
   * @return 0 for success, 1 for failure.
   */
int rhd_generate_waveform(rhd_device_t *dev, int offset, int peak_val, float freq, int sample_per_cycle, void (*delay_us_fn_callback)(int));

/**
 * @brief Read the DAC value from the RHD device.
 *
 * @param dev Pointer to the rhd_device_t structure representing the device.
 * @return The value of the DAC.
 */
uint8_t rhd_read_dac(rhd_device_t *dev);


/**
 * @brief "Force read" a register, sending the "read" command 3 times
 * to get the expected value in the RX buffer.
 *
 * @param dev pointer to rhd_device_t instance
 * @param reg register to read from
 * @return register value
 */
uint8_t rhd_read_force(rhd_device_t *dev, int reg);

/**
 * @brief Run RHD calibration routine
 *
 * @param dev pointer to rhd_device_t instance
 * @return int SPI return code
 */
uint8_t rhd_calib(rhd_device_t *dev);

/**
 * @brief Clear RHD calibration
 *
 * @param dev pointer to rhd_device_t instance
 * @return int SPI return code
 */
uint8_t rhd_clear_calib(rhd_device_t *dev);

/**
 * @brief Read the INTAN registers (40-44) to verify if the chip is working.
 *
 * @param dev pointer to rhd_device_t instance
 * @return int 0 for success. Otherwise, returns the first register which failed.
 */
int rhd_sanity_check(rhd_device_t *dev);

/**
 * @brief Sample RHD2000 channel.
 *
 * @param dev
 * @param ch channel to sample [0-31].
 *
 * @return sample
 */
uint16_t rhd2000_sample(rhd_device_t *dev, uint16_t ch);

/**
 * @brief Sample RHD2164 channels.
 *
 * @param dev
 * @param ch channel to sample [0-31], RHD2164 will also sample all others.
 * @param rx sample reception buffer, where `rx[0]` contains `ch` data and `rx[1]` contains `ch+32` data.
 * @return pointer to sample buffer
 */
uint16_t *rhd2164_sample(rhd_device_t *dev, uint16_t ch, uint16_t *rx);

/**
 * @brief Sample all RHD2164 channels.
 *
 * The values are saved into `sample_buf` at their channel index.alignas
 *
 * Channel 0's LSb is set to 0, while all others are set to 1 for alignment.
 *
 * @param dev pointer to rhd_device_t instance
 */
void rhd2164_sample_all(rhd_device_t *dev, uint16_t *sample_buf);

#endif /* RHD_H */