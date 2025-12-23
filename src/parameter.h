#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include "tp/types.h"

typedef enum modulation_source {
  NONE,
  SPEED,
  COHESION,
  ALIGNMENT,
  AVOIDANCE,
} modulation_source;

typedef struct parameter {
  f32 input_min;
  f32 input_max;

  f32 output_min;
  f32 output_max;

  f32 base;
  f32 modulation;

  f32 amount;

  modulation_source source;
} parameter;

f32 parameter_get(const parameter *p);

#endif
