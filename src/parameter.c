#include "parameter.h"
#include "tp/assert.h"
#include "tp/math.h"

f32 parameter_get(const parameter *p) {
  TP_ASSERT(p);
  f32 modulation_normalised =
      (p->modulation - p->input_min) / (p->input_max - p->input_min);

  modulation_normalised = tp_math_clamp_f32(modulation_normalised, 0.0f, 1.0f);

  f32 modulation_output =
      modulation_normalised * p->amount * (p->output_max - p->output_min) +
      p->output_min;

  return p->base + modulation_output;
}
