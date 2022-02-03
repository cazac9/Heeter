#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <credentials.h>

#define MAX_POWER  3
#define MIN_POWER  1
#define DEFAULT_TEMP 40
#define DEFAULT_POWER 2
#define MIN_WATER_FLOW  20
#define EEPROM_SIZE 3

#define CONFIG_POWER_BYTE 2
#define CONFIG_TEMPERATURE_BYTE 1
#define CONFIG_IS_ON_BYTE 0

enum Command : uint8_t { 
  POWER_UP,
  PARAMS
};

#endif