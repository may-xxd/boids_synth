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

  boids.data[active_boids] =
      (boid){.x = 100.0f, .y = 100.0f, .vx = 1.0f, .vy = 1.0f};
  active_boids++;

  boids.data[active_boids] =
      (boid){.x = 110.0f, .y = 100.0f, .vx = 1.3f, .vy = 1.0f};
  active_boids++;

  boids.data[active_boids] =
      (boid){.x = 110.0f, .y = 110.0f, .vx = 1.5f, .vy = 1.0f};
  active_boids++;

  boids.data[active_boids] =
      (boid){.x = 100.0f, .y = 110.0f, .vx = 1.2f, .vy = 1.0f};
  active_boids++;

  boid_params params = {.avoid = 1.0f, .align = 1.0f, .cohesion = 1.0f};

  InitWindow(WIDTH, HEIGHT, "window_me");

  SetTargetFPS(FPS);

  while (!WindowShouldClose()) {
    f32 dt = GetFrameTime();
    update_boids(boids, active_boids, params, dt);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (usize ii = 0; ii < active_boids; ii++) {
      DrawCircle(boids.data[ii].x, boids.data[ii].y, 5.0f, GRAY);
      DrawLine(boids.data[ii].x, boids.data[ii].y,
               boids.data[ii].x + boids.data[ii].vx,
               boids.data[ii].y + boids.data[ii].vy, RED);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
