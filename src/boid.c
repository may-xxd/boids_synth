#include "boid.h"
#include "tp/assert.h"
#include "tp/math.h"

const f32 PROTECTION_RADIUS = 16.0f;
const f32 VISUAL_RADIUS = 150.0f;
const f32 MARGIN_SIZE = 50.0f;
const f32 TURN_FACTOR = 400.0f;
const f32 ARENA_W = 800.0f;
const f32 ARENA_H = 600.0f;
const f32 MAX_SPEED = 300.0f;
const f32 MIN_SPEED = 30.0f;

void update_boids(tp_slice_boid boids, usize active_boids, boid_params params,
                  f32 dt, f32 mouse_x, f32 mouse_y) {
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

    f32 mouse_dx = b->x - mouse_x;
    f32 mouse_dy = b->y - mouse_y;
    f32 fear_factor = 1.0f /(mouse_dx * mouse_dx + mouse_dy * mouse_dy);
    f32 fear = params.fear * fear_factor;
    b->fear = fear;
    b->vx += dt * mouse_dx * fear;
    b->vy += dt * mouse_dy * fear;

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

    if (speed > MAX_SPEED) {
      b->vx = (b->vx / speed) * MAX_SPEED;
      b->vy = (b->vy / speed) * MAX_SPEED;
      speed = MAX_SPEED;
    }

    else if (speed < MIN_SPEED) {
      b->vx = (b->vx / speed) * MIN_SPEED;
      b->vy = (b->vy / speed) * MIN_SPEED;
      speed = MIN_SPEED;
    }

    b->speed = speed;

    b->x += b->vx * dt;
    b->y += b->vy * dt;

    for (usize module_ii = 0u; module_ii < b->voice.active_modules;
         module_ii++) {
      module *m = &b->voice.modules.data[module_ii];
      switch (m->type) {
      case OSCILLATOR: {
        oscillator *osc = &m->inner.oscillator;
        apply_modulation(b, &osc->freq);
        break;
      }
      case LOW_PASS_FILTER: {
        low_pass_filter *lpf = &m->inner.low_pass_filter;
        apply_modulation(b, &lpf->frequency);
        apply_modulation(b, &lpf->resonance);
        apply_modulation(b, &lpf->drive);
        break;
      }
      }
    }
  }
}

void apply_modulation(boid *b, parameter *p) {
  TP_ASSERT(p);
  switch (p->source) {
  case NONE: {
    return;
  }
  case SPEED: {
    p->modulation = b->speed;
    break;
  }
  case COHESION: {
    p->modulation = b->cohesion;
    break;
  }
  case ALIGNMENT: {
    p->modulation = b->alignment;
    break;
  }
  case AVOIDANCE: {
    p->modulation = b->avoidance;
    break;
  }
  case FEAR: {
    p->modulation = b->fear;
    break;
  }
  }
}

#define NUM_MODULES 10
boid make_boid(f32 x, f32 y, tp_allocator *allocator) {
  boid ret = (boid){.x = x, .y = y};

  ret.voice.modules.data = tp_allocator_alloc(allocator, NUM_MODULES,
                                              sizeof(*ret.voice.modules.data));

  ret.voice.modules.count = NUM_MODULES;
  ret.voice.active_modules = 0;
  return ret;
}

void add_module(boid *b, module m) {
  TP_ASSERT(b);
  TP_ASSERT(b->voice.modules.data);
  TP_ASSERT(b->voice.active_modules < b->voice.modules.count);
  b->voice.modules.data[b->voice.active_modules] = m;
  b->voice.active_modules++;
}
