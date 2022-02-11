#ifndef SHEDULE_MANAGER_H
#define SHEDULE_MANAGER_H

#include <Globals.h>
#include <ArduinoJson.h>

class ScheduleManager {
  public:
    void manage(ParamsMessage &target, ParamsMessage &source);
    vector<vector<ScheduleRange>> parse(JsonObject days);
    ScheduleManager(){};
};

#endif