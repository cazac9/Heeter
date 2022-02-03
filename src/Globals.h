#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <credentials.h>

#define MAX_POWER  3
#define MIN_POWER  1
#define DEFAULT_TEMP 40
#define MIN_WATER_FLOW  20

enum Command : uint8_t { 
  POWER_UP,
  PARAMS,
  DEFAULTS
};
#endif