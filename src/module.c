#include "module.h"
#include "tp/assert.h"
#include "tp/math.h"

void module_apply(module *module, tp_slice_f32 samples, usize sample_rate) {
  TP_ASSERT(module);
  TP_ASSERT(samples.data);

  switch (module->type) {
  case OSCILLATOR: {
    oscillator *osc = &module->inner.oscillator;
    f32 sample_dt = 1.0f / (f32)sample_rate;
    for (usize ii = 0u; ii < samples.count; ii++) {
      switch (osc->shape) {
      case SINE: {
        samples.data[ii] += tp_math_sin_f32(osc->angle) * osc->amplitude;
        break;
      };
      case SAW: {
        samples.data[ii] +=
            (osc->angle - TP_MATH_PI) / TP_MATH_PI * osc->amplitude;
        break;
      }
      }
      osc->angle += 2 * TP_MATH_PI * parameter_get(&osc->freq) * sample_dt;
      if (osc->angle > 2 * TP_MATH_PI) {
        osc->angle -= 2 * TP_MATH_PI;
      }
    }
    break;
  }
  case LOW_PASS_FILTER: {
    low_pass_filter *lpf = &module->inner.low_pass_filter;
    f32 sqrt2 = tp_math_sqrt_f32(2.0f);
    f32 alpha = tp_math_tan_f32(TP_MATH_PI * parameter_get(&lpf->frequency) /
                                sample_rate);
    f32 k = (alpha * alpha) / (1 + sqrt2 * alpha + alpha * alpha);
    f32 a1 = 2 * (alpha * alpha - 1) / (1 + sqrt2 * alpha + alpha * alpha);
    f32 a2 = (1 - sqrt2 * alpha + alpha * alpha) /
             (1 + sqrt2 * alpha + alpha * alpha);
    f32 b1 = 2;
    f32 b2 = 1;
    f32 drive =
        parameter_get(&lpf->drive) > 2.0f ? 2.0f : parameter_get(&lpf->drive);
    drive = drive < 0.0f ? 0.0f : drive;
    for (usize ii = 0; ii < samples.count; ++ii) {
      f32 in_sample = samples.data[ii] * drive;
      f32 out_sample = k * in_sample + k * b1 * lpf->prev_input[0] +
                       k * b2 * lpf->prev_input[1] - a1 * lpf->prev_output[0] -
                       a2 * lpf->prev_output[1];
      out_sample = out_sample * module->wet + in_sample * (1.f - module->wet);
      lpf->prev_input[1] = lpf->prev_input[0];
      lpf->prev_input[0] = in_sample;

      lpf->prev_output[1] = lpf->prev_output[0];
      lpf->prev_output[0] = out_sample;
      samples.data[ii] = out_sample;
    }
    break;
  }
  }
}
