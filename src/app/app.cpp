#include <cstddef>
#include <cstdint>

#include <arm_mve.h>

#include "hal_data.h"

#include "SparkFun_WM8960_Arduino_Library.h"
#include "Wire.h"
#include "config.h"


void setup();
void loop(float *left, float *right);

static volatile size_t ssi_buf_idx = 0;
static volatile bool ssi_buf_rx_ready = false;
static int32_t ssi_buf_rx[2][AUDIO_SAMPLES * AUDIO_CHANNELS];
static int32_t ssi_buf_tx[2][AUDIO_SAMPLES * AUDIO_CHANNELS];

static WM8960 codec;

static void process_audio(const int32_t *input, int32_t *output);
static void i2s_write_read();
static void i2s_callback(i2s_callback_args_t *p_args);

void app() {
  Wire.begin(&g_i2c_master1_ctrl, &g_i2c_master1_cfg);
  while (!codec.begin())
    ;

  // General setup needed
  codec.enableVREF();
  codec.enableVMID();

  codec.setLIN2BOOST(WM8960_BOOST_MIXER_GAIN_0DB);
  codec.setRIN2BOOST(WM8960_BOOST_MIXER_GAIN_0DB);

  // Connect from DAC outputs to output mixer
  codec.enableLD2LO();
  codec.enableRD2RO();

  // Enable output mixers
  codec.enableLOMIX();
  codec.enableROMIX();

  // CLOCK STUFF, These settings will get you 44.1KHz sample rate, and class-d
  // freq at 705.6kHz
  codec.enablePLL(); // Needed for class-d amp clock
  codec.setPLLPRESCALE(WM8960_PLLPRESCALE_DIV_2);
  codec.setSMD(WM8960_PLL_MODE_FRACTIONAL);
  codec.setCLKSEL(WM8960_CLKSEL_PLL);
  codec.setSYSCLKDIV(WM8960_SYSCLK_DIV_BY_2);
  codec.setBCLKDIV(4);
  // codec.setDCLKDIV(WM8960_DCLKDIV_16);
  codec.setPLLN(8);
  codec.setPLLK(0x31, 0x26, 0xE8); // PLLK=86C226h
  // codec.setADCDIV(0); // Default is 000 (what we need for 44.1KHz)
  // codec.setDACDIV(0); // Default is 000 (what we need for 44.1KHz)
  codec.setWL(WM8960_WL_32BIT);

  // Enable ADCs and DACs
  codec.enableAdcLeft();
  codec.enableAdcRight();
  codec.enableDacLeft();
  codec.enableDacRight();
  codec.disableDacMute();

  codec.enableHeadphones();
  codec.enableOUT3MIX(); // Provides VMID as buffer for headphone ground

  codec.setHeadphoneVolumeDB(0.00);

  fsp_err_t err;
  err = R_SSI_Open(&g_i2s0_ctrl, &g_i2s0_cfg);
  while (err)
    ;
  err = R_SSI_CallbackSet(&g_i2s0_ctrl, i2s_callback, nullptr, nullptr);
  while (err)
    ;
  err = R_GPT_Open(&g_timer2_ctrl, &g_timer2_cfg);
  while (err)
    ;
  err = R_GPT_Start(&g_timer2_ctrl);
  while (err)
    ;

  setup();

  i2s_write_read();

  while (true) {
    if (ssi_buf_rx_ready) {
      process_audio(ssi_buf_rx[!ssi_buf_idx], ssi_buf_tx[!ssi_buf_idx]);
      ssi_buf_rx_ready = false;
    }
  }
}

static void process_audio(const int32_t *input, int32_t *output) {
  static float buf_l[AUDIO_SAMPLES] = {};
  static float buf_r[AUDIO_SAMPLES] = {};

  // s32 -> f32
  for (size_t i = 0; i < AUDIO_SAMPLES; i += 4) {
    int32x4x2_t vec_lr_i32 = vld2q(&input[i * 2]);
    float32x4_t vec_l =
        vmulq(vcvtq_f32_s32(vec_lr_i32.val[0]), vdupq_n_f32(1.0f / (1 << 31)));
    float32x4_t vec_r =
        vmulq(vcvtq_f32_s32(vec_lr_i32.val[1]), vdupq_n_f32(1.0f / (1 << 31)));
    vst1q(&buf_l[i], vec_l);
    vst1q(&buf_r[i], vec_r);
  }

  loop(buf_l, buf_r);

  // f32 -> s32
  for (size_t i = 0; i < AUDIO_SAMPLES; i += 4) {
    float32x4_t vec_l = vld1q(&buf_l[i]);
    float32x4_t vec_r = vld1q(&buf_r[i]);
    int32x4x2_t vec_lr_i32 = {
        {
            vcvtq_s32_f32(vmulq(vec_l, vdupq_n_f32(1 << 31))),
            vcvtq_s32_f32(vmulq(vec_r, vdupq_n_f32(1 << 31))),
        },
    };
    vst2q(&output[i * 2], vec_lr_i32);
  }
}

static void i2s_write_read() {
  R_SSI_WriteRead(&g_i2s0_ctrl, ssi_buf_tx[ssi_buf_idx],
                  ssi_buf_rx[ssi_buf_idx], sizeof(ssi_buf_tx[0]));
  ssi_buf_idx = !ssi_buf_idx;
}

static void i2s_callback(i2s_callback_args_t *p_args) {
  if (p_args->event == I2S_EVENT_RX_FULL) {
    ssi_buf_rx_ready = true;
  }
  if (p_args->event == I2S_EVENT_TX_EMPTY || p_args->event == I2S_EVENT_IDLE) {
    i2s_write_read();
  }
}
