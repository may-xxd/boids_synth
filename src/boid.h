#include "tp/slice.h"
#include "tp/types.h"

typedef struct boid {
  f32 x;
  f32 y;
  f32 vx;
  f32 vy;
  f32 avoidance;
  f32 alignment;
  f32 cohesion;
  f32 speed;
} boid;

typedef struct boid_params {
  f32 avoid;
  f32 align;
  f32 cohesion;
} boid_params;

tp_slice_custom(boid);

void update_boids(tp_slice_boid boids, usize active_boids, boid_params params, f32 dt);
