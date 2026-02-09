#pragma once

#include <cstddef>

static constexpr size_t AUDIO_SAMPLES = 256;
static constexpr size_t AUDIO_CHANNELS = 2;

static_assert(AUDIO_SAMPLES % 8 == 0);
