#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

#include "hal_data.h"

#include "cabinet_sim.h"
#include "config.h"
#include "hpf.h"
#include "ir_data.h"

static volatile int mode = 0;

static CabinetSim<AUDIO_SAMPLES, sizeof(IR_DATA) / sizeof(IR_DATA[0])>
    cabinet_sim;
static HPF pre_hpf(48000.0f, 400.0f);
static HPF post_hpf(48000.0f, 250.0f);

static void process_overdrive(float *buf);
static void timer_callback(timer_callback_args_t *);

void setup() {
  R_GPT_Open(&g_timer0_ctrl, &g_timer0_cfg);
  R_GPT_CallbackSet(&g_timer0_ctrl, timer_callback, nullptr, nullptr);
  R_GPT_Start(&g_timer0_ctrl);

  cabinet_sim.init(IR_DATA);
}

void loop(float *left, float *right) {
  switch (mode) {
  case 0:
    for (size_t i = 0; i < AUDIO_SAMPLES; ++i) {
      left[i] *= 2.0f;
    }
    break;
  case 1:
    process_overdrive(left);
    for (size_t i = 0; i < AUDIO_SAMPLES; ++i) {
      left[i] *= 0.05f;
    }
    break;
  case 2:
    process_overdrive(left);
    for (size_t i = 0; i < AUDIO_SAMPLES; ++i) {
      left[i] *= 0.01f;
    }
    cabinet_sim.process(left);
    break;
  default:
    break;
  }

  for (size_t i = 0; i < AUDIO_SAMPLES; ++i) {
    right[i] = left[i];
  }
}

static void process_overdrive(float *buf) {
  for (size_t i = 0; i < AUDIO_SAMPLES; ++i) {
    float x = buf[i];
    x = pre_hpf.process(x);
    x *= 400.0f;
    if (x >= 0.0f) {
      x = std::tanh(4.0f * x) / std::tanh(4.0f);
    } else {
      x = std::tanh(1.0f * x) / std::tanh(1.0f);
    }
    x = post_hpf.process(x);
    buf[i] = x;
  }
}

static void timer_callback(timer_callback_args_t *) {
  static volatile bsp_io_level_t prev_value = BSP_IO_LEVEL_HIGH;
  bsp_io_level_t value;
  R_IOPORT_PinRead(&g_ioport_ctrl, USER_SW, &value);
  if (prev_value == BSP_IO_LEVEL_HIGH && value == BSP_IO_LEVEL_LOW) {
    mode = (mode + 1) % 3;
  }
  prev_value = value;
}