#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

#include <Globals.h>

class ParamsMessage {
  public:
    uint8_t currentTemp;
    uint8_t targetTemp;
    uint8_t power;
    float flow;
    Command command;
    ParamsMessage(){};
};
#endif