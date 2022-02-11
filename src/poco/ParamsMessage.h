#ifndef PARAMS_MESSAGE_H
#define PARAMS_MESSAGE_H

#include <Globals.h>
#include <map>
#include <sstream>
#include <string>
#include <ArduinoJson.h>

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
    String scheduleRaw;
    vector<vector<ScheduleRange>> schedule;

    ParamsMessage(){};

    void parseSchedule(JsonObject days){
      for (uint8_t i = 0; i < 7; i++){
        ostringstream s;
        s << i; 
        JsonArray jranges = days[s.str()].as<JsonArray>();
        vector<ScheduleRange> ranges;
        for (size_t j = 0; j < jranges.size(); j++){
          ScheduleRange range;
          ranges.push_back(range);
          JsonObject jrange = jranges[j].as<JsonObject>();
          ranges[j].start = jrange["s"];
          ranges[j].end = jrange["e"];
          ranges[j].power = jrange["p"];
          ranges[j].targetTemp = jrange["t"];
        }
        schedule.push_back(ranges);
      }
    };

    
};




#endif