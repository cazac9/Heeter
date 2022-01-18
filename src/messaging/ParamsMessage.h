#include <Arduino.h>
#include <Globals.h>

#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

class ParamsMessage {
  public:
    byte currentTemp;
    byte targetTemp;
    byte power;
    Command command;
    ParamsMessage(byte ct, byte tt, byte p, Command cmd);
    ParamsMessage();
};
#endif