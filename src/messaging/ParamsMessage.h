#include <Arduino.h>
#include <Globals.h>

#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

class ParamsMessage {
  public:
    byte currentTemp;
    byte targetTemp;
    byte power;
    String dateTime;
    Command command;
    ParamsMessage(){};
};
#endif