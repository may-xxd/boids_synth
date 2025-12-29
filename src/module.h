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

typedef struct oscillator {
  f32 angle;
  parameter freq;
  oscillator_shape shape;
  f32 amplitude;
} oscillator;

typedef struct low_pass_filter {
  parameter frequency;
  parameter resonance;
  parameter drive;
  f32 prev_input[2];
  f32 prev_output[2];
} low_pass_filter;

typedef struct module {
  module_type type;
  f32 wet;
  union {
    oscillator oscillator;
    low_pass_filter low_pass_filter;
  } inner;
} module;

tp_slice_custom(module);

void module_apply(module *module, tp_slice_f32 samples, usize sample_rate);

#endif
