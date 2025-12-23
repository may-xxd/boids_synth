#ifndef __VOICE_H__
#define __VOICE_H__

#include "module.h"

typedef struct voice {
  tp_slice_module modules;
  usize active_modules;
} voice;

#endif
