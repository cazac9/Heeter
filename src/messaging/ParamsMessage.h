#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

#include <Globals.h>
#include <map>
#include <ArduinoJson.h>

class ParamsMessage {
  public:
    uint8_t currentTemp;
    uint8_t targetTemp;
    uint8_t power;
    uint8_t flow;
    uint8_t isOn;
    uint8_t isOnSchedule;
    Command command;
    const char * scheduleRaw;
    std::map<uint8_t, ScheduleRange *> schedule;

    ParamsMessage(){};

    void parseSchedule(DynamicJsonDocument doc){
      JsonArray days = doc.as<JsonArray>();
      for (size_t i = 0; i < 7; i++){
        JsonArray jranges = days[i].as<JsonArray>();
        ScheduleRange * ranges = (ScheduleRange*)malloc(sizeof(ScheduleRange) * jranges.size());

        for (size_t j = 0; j < jranges.size(); j++){
          JsonObject jrange = jranges[j].as<JsonObject>();
          ScheduleRange range = ranges[j];
          range.start = jrange["s"];
          range.end = jrange["e"];
          range.power = jrange["p"];
          range.targetTemp = jrange["t"];
        }
        
        schedule.insert(std::make_pair(i, ranges));
      }
    };

    
};




#endif