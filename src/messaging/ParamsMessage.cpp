#include <Arduino.h>
#include <messaging/ParamsMessage.h>
#include <Globals.h>


ParamsMessage::ParamsMessage(byte ct, byte tt, byte p, Command cmd){
  currentTemp = ct;
  targetTemp =  tt;
  power = p;
  command = cmd;
}

ParamsMessage::ParamsMessage(){}