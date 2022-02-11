#ifndef GLOBALS_H
#define GLOBALS_H

#include <Arduino.h>
#include <credentials.h>
#include <poco/Command.h>
#include <poco/ScheduleRange.h>
#include <poco/ParamsMessage.h>

#define MAX_POWER  3
#define MIN_POWER  1
#define DEFAULT_TEMP 40
#define DEFAULT_POWER 2
#define MIN_WATER_FLOW  20
#define PARAMS_MESSAGE_SIZE 4000

#define CONFIG_IS_ON_BYTE 0
#define CONFIG_TEMPERATURE_BYTE 1
#define CONFIG_POWER_BYTE 2
#define CONFIG_IS_ON_SCHEDULE_BYTE 3
#define CONFIG_SCHEDULE_BYTE 4

#endif