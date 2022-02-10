#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

#include <Globals.h>
#include <map>
#include <sstream>
#include <string>
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

    void parseSchedule(JsonObject days){
      for (uint8_t i = 0; i < 7; i++){
        std::ostringstream s;
        s << i; 
        JsonArray jranges = days[s.str()].as<JsonArray>();
        ScheduleRange * ranges = (ScheduleRange*)malloc(sizeof(ScheduleRange) * jranges.size());
        for (size_t j = 0; j < jranges.size(); j++){
          JsonObject jrange = jranges[j].as<JsonObject>();
          ranges[j].start = jrange["s"];
          ranges[j].end = jrange["e"];
          ranges[j].power = jrange["p"];
          ranges[j].targetTemp = jrange["t"];
        }
        Serial.println("insert");
        schedule.insert(std::make_pair(i, ranges));
      }
    };

    
};




#endif