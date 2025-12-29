#ifndef __BOID_H__
#define __BOID_H__

#include "tp/allocator.h"
#include "tp/slice.h"
#include "tp/types.h"
#include "voice.h"

typedef struct boid {
  f32 x;
  f32 y;
  f32 vx;
  f32 vy;
  f32 avoidance;
  f32 alignment;
  f32 cohesion;
  f32 speed;
  f32 fear;
  voice voice;
} boid;

typedef struct boid_params {
  f32 avoid;
  f32 align;
  f32 cohesion;
  f32 fear;
} boid_params;

tp_slice_custom(boid);

boid make_boid(f32 x, f32 y, tp_allocator *allocator);
void add_module(boid *b, module m);
void apply_modulation(boid *b, parameter *p);

void update_boids(tp_slice_boid boids, usize active_boids, boid_params params,
                  f32 dt, f32 mouse_x, f32 mouse_y);
#endif
