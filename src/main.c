#include "boid.h"
#include "raylib.h"
#include "tp/assert.h"
#include "tp/audio.h"
#include "tp/camera.h"
#include "tp/image.h"
#include "tp/io.h"
#include "tp/log.h"
#include "tp/math.h"
#include "tp/string.h"
#include "tp/time.h"

int main(void) {
  tp_allocator allocator = tp_allocator_create(
      tp_allocator_virtual_memory_alloc(1024 * 1024 * 1024));
  (void)allocator;

  const usize WIDTH = 800;
  const usize HEIGHT = 600;
  const usize FPS = 60;
  const usize MAX_BOIDS = 1024;

  tp_slice_boid boids;
  boids.data = tp_allocator_alloc(&allocator, MAX_BOIDS, sizeof(boid));
  boids.count = MAX_BOIDS;
  usize active_boids = 0;
  boid_params params = {.avoid = 1.0f, .align = 1.0f, .cohesion = 1.0f};

  InitWindow(WIDTH, HEIGHT, "window_me");

  SetTargetFPS(FPS);

  while (!WindowShouldClose()) {
    f32 dt = GetFrameTime();
    update_boids(boids, active_boids, params, dt);
  }

  CloseWindow();

  return 0;
}
