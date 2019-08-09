/* Copyright 2013-2017 Matt Tytel
 *
 * mopo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * mopo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with mopo.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fixed_point_wave.h"

#include <string>
#include <cstring>
#include <random>

namespace mopo {

  FixedPointWaveLookup::FixedPointWaveLookup() {
    preprocessSin();
    preprocessCustom();
    preprocessTriangle();
    preprocessSquare();
    preprocessUpSaw();
    preprocessDownSaw();
    preprocessStep<3>(three_step_);
    preprocessStep<4>(four_step_);
    preprocessStep<8>(eight_step_);
    preprocessPyramid<3>(three_pyramid_);
    preprocessPyramid<5>(five_pyramid_);
    preprocessPyramid<9>(nine_pyramid_);

    wave_type waves[kNumFixedPointWaveforms] =
        { sin_, triangle_, square_, down_saw_, up_saw_,
          three_step_, four_step_, eight_step_,
          three_pyramid_, five_pyramid_, nine_pyramid_ };

    memcpy(waves_, waves, kNumFixedPointWaveforms * sizeof(wave_type));
  }

  void FixedPointWaveLookup::preprocessSin() {
    for (int h = 0; h < HARMONICS + 1; ++h) {
      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i)
        sin_[h][i] = sin((2 * PI * i) / FIXED_LOOKUP_SIZE);
    }

    preprocessDiffs(sin_);
  }

  void FixedPointWaveLookup::preprocessTriangle() {
    for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
      triangle_[0][i] = Wave::triangle((1.0 * i) / FIXED_LOOKUP_SIZE);

      int p = i;
      mopo_float scale = 8.0 / (PI * PI);
      triangle_[HARMONICS][i] = scale * sin_[0][p];

      for (int h = 1; h < HARMONICS; ++h) {
        p = (p + i) % FIXED_LOOKUP_SIZE;
        triangle_[HARMONICS - h][i] = triangle_[HARMONICS - h + 1][i];
        mopo_float harmonic = scale * sin_[0][p] / ((h + 1) * (h + 1));

        if (h % 4 == 0)
          triangle_[HARMONICS - h][i] += harmonic;
        else if (h % 2 == 0)
          triangle_[HARMONICS - h][i] -= harmonic;
      }
    }

    preprocessDiffs(triangle_);
  }

  void FixedPointWaveLookup::preprocessSquare() {
    for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
      square_[0][i] = Wave::square((1.0 * i) / FIXED_LOOKUP_SIZE);

      int p = i;
      mopo_float scale = 4.0 / PI;
      square_[HARMONICS][i] = scale * sin_[0][p];

      for (int h = 1; h < HARMONICS; ++h) {
        p = (p + i) % FIXED_LOOKUP_SIZE;
        square_[HARMONICS - h][i] = square_[HARMONICS - h + 1][i];

        if (h % 2 == 0)
          square_[HARMONICS - h][i] += scale * sin_[0][p] / (h + 1);
      }
    }

    preprocessDiffs(square_);
  }

  void FixedPointWaveLookup::preprocessDownSaw() {
    for (int h = 0; h < HARMONICS + 1; ++h) {
      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i)
        down_saw_[h][i] = -up_saw_[h][i];
    }

    preprocessDiffs(down_saw_);
  }

  void FixedPointWaveLookup::writeOutSvg(
    const std::string& filename,
    wave_type wave
  ) {

    // Write out the result to an SVG file.
    SvgOutput svg(filename);
    svg.set_scale(100, true);
    svg.set_stroke(0x000000, 1, 0.5);
    svg.set_fill(0x000000, 0);

    for (int h = 0; h < HARMONICS; ++h) {
      std::vector<SvgOutput::Point2> waveform;
      for (int i = 0; i < FIXED_LOOKUP_SIZE; i+=1) {
        SvgOutput::Point2 begin;
        begin.x = double(i) / FIXED_LOOKUP_SIZE + 1.5*h;
        begin.y = wave[h][i];

        waveform.push_back(begin);
      }
      svg.write_path("h" + std::to_string(h), waveform);
    }


    svg.set_stroke(0x999999, 1);
    svg.write_line("plus_one", SvgOutput::Point2(0, 1), SvgOutput::Point2(HARMONICS*1.5 ,1));
    svg.write_line("zero", SvgOutput::Point2(0, 0), SvgOutput::Point2(HARMONICS*1.5 ,0));
    svg.write_line("minus_one", SvgOutput::Point2(0, -1), SvgOutput::Point2(HARMONICS*1.5 ,-1));
  }

  void FixedPointWaveLookup::preprocessUpSaw() {
    return;

    for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
      up_saw_[0][i] = Wave::upsaw((1.0 * i) / FIXED_LOOKUP_SIZE);

      int index = (i + (FIXED_LOOKUP_SIZE / 2)) % FIXED_LOOKUP_SIZE;
      int p = i;
      mopo_float scale = 2.0 / PI;
      up_saw_[HARMONICS][index] = scale * sin_[0][p];

      for (int h = 1; h < HARMONICS; ++h) {
        //up_saw_[HARMONICS - h][index] = up_saw_[HARMONICS][index];

        p = (p + i) % FIXED_LOOKUP_SIZE;
        mopo_float harmonic = scale * sin_[0][p] / (h + 1);

        if (h % 2 == 0)
          up_saw_[HARMONICS - h][index] = up_saw_[HARMONICS - h + 1][index] + harmonic;
        else
          up_saw_[HARMONICS - h][index] = up_saw_[HARMONICS - h + 1][index] - harmonic;
      }
    }

    preprocessDiffs(up_saw_);

    writeOutSvg("D:/temp/helm_up_saw.svg", up_saw_);
  }


  mopo_float Wave::s_custom_lookup[2 * 1024];
  void FixedPointWaveLookup::preprocessCustom() {
    auto& wave = up_saw_;

    enum class CustomStrategy {
      UP_SAW,
      RANDOM,
      RANDOM_10,
    } custom_strategy = CustomStrategy::RANDOM;
    std::default_random_engine generator;
    std::uniform_real_distribution<mopo_float> distribution(-1,1);
    switch (custom_strategy) {
      case CustomStrategy::UP_SAW: {
        mopo_float scale = 2.0 / PI;
        for (int hw = 0; hw < HARMONICS+1; ++hw) {
          if (hw % 2 == 0) {
            custom_harmonic_weights_[hw] = scale / (hw + 1);
          } else {
            custom_harmonic_weights_[hw] = -scale / (hw + 1);
          }
        }
      } break;
      case CustomStrategy::RANDOM: {
        for (int hw = 0; hw < HARMONICS+1; ++hw) {
          custom_harmonic_weights_[hw] = distribution(generator) / (hw*hw + 1);
        }
      } break;
      case CustomStrategy::RANDOM_10: {
        for (int hw = 0; hw < 10; ++hw) {
          custom_harmonic_weights_[hw] = distribution(generator);
        }
      } break;
    }


    for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
      int index = (i + (FIXED_LOOKUP_SIZE / 2)) % FIXED_LOOKUP_SIZE;
      int p = i;
      wave[HARMONICS][index] = custom_harmonic_weights_[0] * sin_[0][p];

      for (int h = 1; h < HARMONICS+1; ++h) {
        p = (p + i) % FIXED_LOOKUP_SIZE;
        mopo_float harmonic = custom_harmonic_weights_[h] * sin_[0][p];
        wave[HARMONICS - h][index] = wave[HARMONICS - h + 1][index] + harmonic;
      }
    }

    // Normalize
    for (int h = 0; h <= HARMONICS; ++h) {
      mopo_float peak = 0;
      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
        if (peak < wave[h][i]) {
          peak = wave[h][i];
        }
      }
      if (peak == 0) break;
      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
        wave[h][i] /= peak;
      }
    }

//    preprocessDiffs(wave);

    for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
      Wave::s_custom_lookup[i] = wave[0][i];
    }

    writeOutSvg("D:/temp/helm_custom.svg", wave);
  }

  template<size_t steps>
  void FixedPointWaveLookup::preprocessStep(wave_type buffer) {
    static int num_steps = steps;
    static const mopo_float step_size = num_steps / (num_steps - 1.0);

    for (int h = 0; h < HARMONICS + 1; ++h) {
      int base_num_harmonics = HARMONICS + 1 - h;
      int harmony_num_harmonics = base_num_harmonics / num_steps;
      int harmony_h = HARMONICS + 1 - harmony_num_harmonics;

      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
        buffer[h][i] = step_size * up_saw_[h][i];

        if (harmony_num_harmonics) {
          int harm_index = (num_steps * i) % FIXED_LOOKUP_SIZE;
          buffer[h][i] += step_size * down_saw_[harmony_h][harm_index] / num_steps;
        }
      }
    }

    preprocessDiffs(buffer);
  }

  template<size_t steps>
  void FixedPointWaveLookup::preprocessPyramid(wave_type buffer) {
    static const int squares = steps - 1;
    static const int offset = 3 * FIXED_LOOKUP_SIZE / 4;

    for (int h = 0; h < HARMONICS + 1; ++h) {
      for (int i = 0; i < FIXED_LOOKUP_SIZE; ++i) {
        buffer[h][i] = 0;

        for (size_t s = 0; s < squares; ++s) {
          int square_offset = (s * FIXED_LOOKUP_SIZE) / (2 * squares);
          int phase = (i + offset + square_offset) % FIXED_LOOKUP_SIZE;
          buffer[h][i] += square_[h][phase] / squares;
        }
      }
    }

    preprocessDiffs(buffer);
  }

  void FixedPointWaveLookup::preprocessDiffs(wave_type wave) {
    for (int h = 0; h < HARMONICS + 1; ++h) {
      for (int i = 0; i < FIXED_LOOKUP_SIZE - 1; ++i)
        wave[h][i + FIXED_LOOKUP_SIZE] = FRACTIONAL_MULT * (wave[h][i + 1] - wave[h][i]);

      mopo_float last_delta = wave[h][0] - wave[h][FIXED_LOOKUP_SIZE - 1];
      wave[h][2 * FIXED_LOOKUP_SIZE - 1] = FRACTIONAL_MULT * last_delta;
    }
  }

  const FixedPointWaveLookup FixedPointWave::lookup_;
} // namespace mopo
