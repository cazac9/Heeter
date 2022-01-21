#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>

#define MAX_POWER  3
#define MIN_POWER  1
#define DEFAULT_TEMP 40

enum Command : byte { 
  CT_SET,
  TT_SET,
  POWER_UP,
  DEFAULTS
};
#endif