#pragma once

#include <algorithm>
#include <array>
#include <complex>
#include <cstddef>

#include "rfft.h"

template <size_t BLOCK_LEN, size_t IR_LEN> class CabinetSim {
public:
  void init(const float *ir_data) {
    rfft_.init(N);

    // IRをブロックサイズに分割してFFT事前計算
    for (size_t i = 0; i < ir_freq_.size(); ++i) {
      std::fill(buf_time_.begin(), buf_time_.end(), 0.0f);
      std::copy(ir_data + BLOCK_LEN * i, ir_data + BLOCK_LEN * (i + 1),
                buf_time_.begin());
      rfft_.forward(buf_time_.data(), ir_freq_[i].data());
    }
  }

  void process(float *block) {
    // overlap-save
    auto itr = std::copy(overlap_.begin(), overlap_.end(), buf_time_.begin());
    itr = std::copy(block, block + BLOCK_LEN, itr);
    std::fill(itr, buf_time_.end(), 0.0f);

    rfft_.forward(buf_time_.data(), block_freq_[block_freq_idx_].data());

    // accumulate
    std::fill(buf_freq_.begin(), buf_freq_.end(), 0.0f);
    for (size_t i = 0; i < ir_freq_.size(); ++i) {
      auto &x = block_freq_[(block_freq_idx_ + ir_freq_.size() - i) %
                            ir_freq_.size()];
      auto &h = ir_freq_[i];

      buf_freq_[0] += x[0] * h[0]; // dc
      buf_freq_[1] += x[1] * h[1]; // nyquist

      for (size_t j = 2; j < N; j += 2) {
        auto conv = std::complex<float>{x[j], x[j + 1]} *
                    std::complex<float>{h[j], h[j + 1]};
        buf_freq_[j] += conv.real();
        buf_freq_[j + 1] += conv.imag();
      }
    }

    rfft_.inverse(buf_freq_.data(), buf_time_.data());

    // overlap-save
    std::copy(block + 1, block + BLOCK_LEN, overlap_.begin());
    std::copy(buf_time_.begin() + BLOCK_LEN - 1,
              buf_time_.begin() + BLOCK_LEN * 2 - 1, block);

    block_freq_idx_ = (block_freq_idx_ + 1) % ir_freq_.size();
  }

private:
  static_assert((BLOCK_LEN & (BLOCK_LEN - 1)) == 0);
  static_assert(IR_LEN % BLOCK_LEN == 0);

  static constexpr size_t N = BLOCK_LEN * 2;

  RFFT<float> rfft_;

  std::array<float, N> buf_time_{};
  std::array<float, N> buf_freq_{};
  std::array<std::array<float, N>, IR_LEN / BLOCK_LEN> ir_freq_{};
  std::array<std::array<float, N>, IR_LEN / BLOCK_LEN> block_freq_{};
  std::array<float, BLOCK_LEN - 1> overlap_{};

  size_t block_freq_idx_ = 0;
};
