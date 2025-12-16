#include "boid.h"
#include "tp/assert.h"
#include "tp/math.h"

const f32 PROTECTION_RADIUS = 16.0f;
const f32 VISUAL_RADIUS = 150.0f;
const f32 MARGIN_SIZE = 50.0f;
const f32 TURN_FACTOR = 400.0f;
const f32 ARENA_W = 800.0f;
const f32 ARENA_H = 600.0f;

void update_boids(tp_slice_boid boids, usize active_boids, boid_params params,
                  f32 dt) {
  TP_ASSERT(boids.data);
  for (usize ii = 0; ii < active_boids; ii++) {
    boid *b = &boids.data[ii];
    uint32_t neighbouring_boids = 0;

    // summed for avoidance
    float dx_sum = 0.0f;
    float dy_sum = 0.0f;

    // summed for alignment
    float vx_sum = 0.0f;
    float vy_sum = 0.0f;

    // summed for cohesion
    float x_sum = 0.0f;
    float y_sum = 0.0f;

    for (usize jj = 0; jj < active_boids; jj++) {
      if (jj == ii) {
        continue;
      }
      boid *other_b = &boids.data[jj];
      float dx = b->x - other_b->x;
      float dy = b->y - other_b->y;

      float distance2 = dx * dx + dy * dy;

      if (distance2 < PROTECTION_RADIUS * PROTECTION_RADIUS) {
        if (&b != &other_b) {
          dx_sum += dx;
          dy_sum += dy;
        }
      }

      else if (distance2 < VISUAL_RADIUS * VISUAL_RADIUS) {
        neighbouring_boids += 1;

        vx_sum += other_b->vx;
        vy_sum += other_b->vy;

        x_sum += other_b->x;
        y_sum += other_b->y;
      }
    }

    float neighbouring_boids_f = (neighbouring_boids);

    b->vx += dt * dx_sum * params.avoid;
    b->vy += dt * dy_sum * params.avoid;
    b->avoidance =
        params.avoid * tp_math_sqrt_f32(dx_sum * dx_sum + dy_sum * dy_sum);

    if (neighbouring_boids_f != 0) {
      float alignment_x = vx_sum / neighbouring_boids_f - b->vx;
      float alignment_y = vy_sum / neighbouring_boids_f - b->vy;
      b->vx += dt * alignment_x * params.align;
      b->vy += dt * alignment_y * params.align;
      b->alignment = params.align * tp_math_sqrt_f32(alignment_x * alignment_x +
                                                     alignment_y * alignment_y);

      float cohesion_x = x_sum / neighbouring_boids_f - b->x;
      float cohesion_y = y_sum / neighbouring_boids_f - b->y;
      b->vx += dt * cohesion_x * params.cohesion;
      b->vy += dt * cohesion_y * params.cohesion;
      b->cohesion = params.cohesion * tp_math_sqrt_f32(cohesion_x * cohesion_x +
                                                       cohesion_y * cohesion_y);
    }

    if (b->x < MARGIN_SIZE) {
      b->vx += dt * TURN_FACTOR;
    } else if (b->x > ARENA_W - MARGIN_SIZE) {
      b->vx -= dt * TURN_FACTOR;
    }

    if (b->y < MARGIN_SIZE) {
      b->vy += dt * TURN_FACTOR;
    } else if (b->y > ARENA_H - MARGIN_SIZE) {
      b->vy -= dt * TURN_FACTOR;
    }

    float speed = tp_math_sqrt_f32(b->vx * b->vx + b->vy * b->vy);

    /*
    if (speed > max_speed) {
      b.vx = (b.vx / speed) * max_speed;
      b.vy = (b.vy / speed) * max_speed;
      speed = max_speed;
    }

    else if (speed < min_speed) {
      b.vx = (b.vx / speed) * min_speed;
      b.vy = (b.vy / speed) * min_speed;
      speed = min_speed;
    }
    */

    b->speed = speed;

    b->x += b->vx * dt;
    b->y += b->vy * dt;
  }
}
