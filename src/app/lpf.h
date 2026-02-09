#pragma once

#include <cmath>

class LPF {
public:
  LPF(float sample_rate, float cutoff)
      : sample_rate_(sample_rate), cutoff_(cutoff) {}

  float process(float x) {
    float tau = 1.0f / (2.0f * M_PI * cutoff_);
    float dt = 1.0f / sample_rate_;
    float alpha = 1.0f - std::exp(-dt / tau);
    float y = alpha * x + (1.0f - alpha) * prev_;
    prev_ = y;
    return y;
  }

private:
  float sample_rate_;
  float cutoff_;
  float prev_ = 0.0f;
};
