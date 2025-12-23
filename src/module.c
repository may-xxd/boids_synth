#include "module.h"
#include "tp/assert.h"
#include "tp/math.h"

void module_apply(module *module, tp_slice_f32 samples, usize sample_rate) {
  TP_ASSERT(module);
  TP_ASSERT(samples.data);

  switch (module->type) {
  case OSCILLATOR: {
    oscillator *osc = &module->inner.oscillator;
    switch (osc->shape) {
    case SINE: {
      f32 sample_dt = 1.0f / (f32)sample_rate;
      for (usize ii = 0u; ii < samples.count; ii++) {
        samples.data[ii] += tp_math_sin_f32(osc->angle) * osc->amplitude;
        osc->angle += 2 * TP_MATH_PI * parameter_get(&osc->freq) * sample_dt;
        if (osc->angle > 2 * TP_MATH_PI) {
          osc->angle -= 2 * TP_MATH_PI;
        }
      }
      break;
    }
    }
    break;
  }
  case LOW_PASS_FILTER: {
    // no-op for now
    break;
  }
  }
}
