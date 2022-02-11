#include <map>
#include <sstream>
#include <string>
#include <ArduinoJson.h>
#include <Globals.h>
#include <ScheduleManager.h>

void ScheduleManager::manage(ParamsMessage &target, ParamsMessage &source){
  bool isOnSchedule = source.isOnSchedule == 1
     || (target.isOnSchedule == 1 && source.isOnSchedule != 2);
  
  if(!isOnSchedule)
    return;
  vector<vector<ScheduleRange>> schedule = source.schedule;
  if (schedule.size() == 0)
    schedule = target.schedule;
   
  struct tm timeinfo;
  if(schedule.size() > 0  && getLocalTime(&timeinfo)){
    target.schedule = schedule;
    float timenow = timeinfo.tm_hour + (float)(timeinfo.tm_min / 60);
    vector<ScheduleRange> daylySchedule  = schedule[timeinfo.tm_wday];
    for (size_t i = 0; i < 10; i++)
    {
      if(timenow >= daylySchedule[i].start && timenow <= daylySchedule[i].end){
        target.targetTemp = daylySchedule[i].targetTemp;
        target.power = daylySchedule[i].power;
        break;
      }
    }
  }
}

vector<vector<ScheduleRange>> ScheduleManager::parse(JsonObject days){
  vector<vector<ScheduleRange>> schedule;
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

  return schedule;
};