#include "boid.h"
#include "raylib.h"
#include "tp/assert.h"
#include "tp/audio.h"
#include "tp/camera.h"
#include "tp/image.h"
#include "tp/io.h"
#include "tp/log.h"
#include "tp/math.h"
#include "tp/music.h"
#include "tp/string.h"
#include "tp/time.h"

#define MAX_SAMPLES 65536

int main(void) {
  tp_allocator allocator = tp_allocator_create(
      tp_allocator_virtual_memory_alloc(1024 * 1024 * 1024));

  const usize WIDTH = 800;
  const usize HEIGHT = 600;
  const usize FPS = 60;
  const usize MAX_BOIDS = 1024;
  const usize SAMPLE_RATE = 44100;

  tp_audio audio =
      tp_audio_create(&allocator, tp_string_from_string_constant("boids_synth"),
                      1, SAMPLE_RATE);

  tp_slice_boid boids;
  boids.data = tp_allocator_alloc(&allocator, MAX_BOIDS, sizeof(boid));
  boids.count = MAX_BOIDS;
  usize active_boids = 0;

  oscillator o = {.freq = {.input_min = 0,
                           .input_max = 100.f,
                           .output_min = 0.0f,
                           .output_max = 50.0f,
                           .base = tp_music_note_to_hz(NOTE_C, 4),
                           .amount = 0.5f,
                           .source = SPEED},
                  .shape = SAW,
                  .amplitude = 1.0f};
  module m_o = {.type = OSCILLATOR, .inner = {.oscillator = o}, .wet = 1.0f};

  low_pass_filter lpf = {
      .frequency = {.input_min = 0.0f,
                    .input_max = 10.0f,
                    .output_min = 0.0f,
                    .output_max = 1000.0f,
                    .base = 400.0f,
                    .amount = 1.0f,
                    .source = FEAR},
      .resonance = {.input_min = 0.0f,
                    .input_max = 10.0f,
                    .output_min = 0.0f,
                    .output_max = 50.0f,
                    .base = 0.9f / 4.9f,
                    .amount = 1.0f,
                    .source = NONE},
      .drive = {.input_min = 0.0f,
                .input_max = 10.0f,
                .output_min = 0.0f,
                .output_max = 50.0f,
                .base = 1.0f,
                .amount = 1.0f,
                .source = NONE},
  };

  module m_f = {
      .type = LOW_PASS_FILTER, .inner = {.low_pass_filter = lpf}, .wet = 1.0f};

  (void)m_f;
  for (u8 octave = 2; octave < 6; octave++) {
    boids.data[active_boids] = make_boid(100.0f, 100.0f, &allocator);
    m_o.inner.oscillator.freq.base = tp_music_note_to_hz(NOTE_C, octave);
    add_module(&boids.data[active_boids], m_o);
    add_module(&boids.data[active_boids], m_f);
    active_boids++;

    boids.data[active_boids] = make_boid(110.0f, 100.0f, &allocator);
    m_o.inner.oscillator.freq.base = tp_music_note_to_hz(NOTE_E, octave);
    add_module(&boids.data[active_boids], m_o);
    add_module(&boids.data[active_boids], m_f);
    active_boids++;

    boids.data[active_boids] = make_boid(110.0f, 110.0f, &allocator);
    m_o.inner.oscillator.freq.base = tp_music_note_to_hz(NOTE_A, octave);
    add_module(&boids.data[active_boids], m_o);
    add_module(&boids.data[active_boids], m_f);
    active_boids++;

    boids.data[active_boids] = make_boid(100.0f, 110.0f, &allocator);
    m_o.inner.oscillator.freq.base = tp_music_note_to_hz(NOTE_B, octave);
    add_module(&boids.data[active_boids], m_o);
    add_module(&boids.data[active_boids], m_f);
    active_boids++;
  }

  boid_params params = {
      .avoid = 1.0f, .align = 1.0f, .cohesion = 1.0f, .fear = 3000.0f};

  InitWindow(WIDTH, HEIGHT, "window_me");

  SetTargetFPS(FPS);

  f32 dt = 1.0f / FPS;
  usize frame_ii = 0;
  while (!WindowShouldClose()) {
    Vector2 mouse_pos = GetMousePosition();
    f32 samples_f32[MAX_SAMPLES] = {0};
    i16 samples_i16[MAX_SAMPLES] = {0};
    usize num_samples = dt * SAMPLE_RATE + 1;
    TP_ASSERT(num_samples < MAX_SAMPLES);
    tp_slice_f32 samples_slice = {.data = samples_f32, .count = num_samples};

    tp_slice_i16 samples_slice_i16 = {.data = samples_i16,
                                      .count = num_samples};

    update_boids(boids, active_boids, params, dt, mouse_pos.x, mouse_pos.y);
    BeginDrawing();
    ClearBackground(RAYWHITE);
    for (usize ii = 0; ii < active_boids; ii++) {
      boid *b = &boids.data[ii];
      DrawCircle(b->x, b->y, 5.0f, GRAY);
      DrawLine(b->x, b->y, b->x + b->vx, b->y + b->vy, RED);

      for (usize jj = 0; jj < b->voice.active_modules; jj++) {
        module_apply(&b->voice.modules.data[jj], samples_slice, SAMPLE_RATE);
      }
    }
    EndDrawing();

    for (usize jj = 0; jj < num_samples; jj++) {
      samples_slice_i16.data[jj] =
          samples_slice.data[jj] / active_boids * 32767;
    }

    tp_audio_write(&allocator, audio, samples_slice_i16);

    if (frame_ii == 0) {
      tp_audio_write_wav(&allocator, samples_slice_i16,
                         tp_string_from_string_constant("frame_0.wav"), 1,
                         SAMPLE_RATE);
    }
    if (frame_ii == 1) {
      tp_audio_write_wav(&allocator, samples_slice_i16,
                         tp_string_from_string_constant("frame_1.wav"), 1,
                         SAMPLE_RATE);
    }

    dt = GetFrameTime();
    frame_ii++;
  }

  CloseWindow();

  return 0;
}
