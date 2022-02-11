#ifndef SHEDULE_MANAGER_H
#define SHEDULE_MANAGER_H

#include <Globals.h>

class ScheduleManager {
  public:
    void manage(ParamsMessage &target, ParamsMessage &source);
    vector<vector<ScheduleRange>> parse(String json);
    ScheduleManager(){};
};

#endif