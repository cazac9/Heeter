#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <Globals.h>
#include <ScheduleManager.h>

class ConfigurationManager {
  public:
    ParamsMessage load();
    void save(ParamsMessage paramsMsg, ParamsMessage controlMsg);
    ConfigurationManager(){};
  private:
    ScheduleManager schedule;
    void saveConfig(uint8_t receivedValue, uint8_t currentValue, uint8_t storage);
};

#endif