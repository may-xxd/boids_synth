#ifndef __MODULE_H__
#define __MODULE_H__

#include "parameter.h"
#include "tp/slice.h"
typedef enum module_type {
  OSCILLATOR,
  LOW_PASS_FILTER,
} module_type;

typedef enum oscillator_shape {
  SINE,
} oscillator_shape;

typedef struct oscillator_params {
  f32 angle;
  parameter freq;
  oscillator_shape shape;
  f32 amplitude;
} oscillator;

typedef struct low_pass_filter_params {
  parameter frequency;
  parameter resonance;
  parameter drive;
} low_pass_filter;

typedef struct module {
  module_type type;
  union {
    oscillator oscillator;
    low_pass_filter low_pass_filter;
  } inner;
} module;

tp_slice_custom(module);

void module_apply(module *module, tp_slice_f32 samples, usize sample_rate);

#endif
