#pragma once

#include "lpf.h"

class HPF {
public:
  HPF(float sample_rate, float cutoff) : lpf_(sample_rate, cutoff) {}

  float process(float x) { return x - lpf_.process(x); }

private:
  LPF lpf_;
};
