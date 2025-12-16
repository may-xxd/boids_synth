#include "tp/assert.h"
#include "tp/audio.h"
#include "tp/camera.h"
#include "tp/image.h"
#include "tp/io.h"
#include "tp/log.h"
#include "tp/math.h"
#include "tp/string.h"
#include "tp/time.h"
#include <emmintrin.h>

int main(void) {
  tp_allocator allocator = tp_allocator_create(
      tp_allocator_virtual_memory_alloc(1024 * 1024 * 1024));

  (void)allocator;
  return 0;
}
