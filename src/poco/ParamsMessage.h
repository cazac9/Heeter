#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

#include <Globals.h>

using namespace std;

class ParamsMessage {
  public:
    uint8_t currentTemp;
    uint8_t targetTemp;
    uint8_t power;
    uint8_t flow;
    uint8_t isOn;
    uint8_t isOnSchedule;
    Command command;
    char * scheduleRaw;
    vector<vector<ScheduleRange>> schedule;

    ParamsMessage(){
      scheduleRaw = nullptr;
    };
};




#endif