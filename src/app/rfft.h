#pragma once

#include <cstddef>

#include <arm_math.h>

template <class T> class RFFT;

template <> class RFFT<float> {
public:
  void init(size_t n) {
    arm_status err = arm_rfft_fast_init_f32(&rfft_, n);
    while (err)
      ;
  }

  void forward(float *input, float *output) {
    arm_rfft_fast_f32(&rfft_, input, output, 0);
  }

  void inverse(float *input, float *output) {
    arm_rfft_fast_f32(&rfft_, input, output, 1);
  }

private:
  arm_rfft_fast_instance_f32 rfft_;
};