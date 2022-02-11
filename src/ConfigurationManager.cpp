#include <Globals.h>
#include <EEPROM.h>
#include <ConfigurationManager.h>
#include <ScheduleManager.h>

ScheduleManager schedule;

ConfigurationManager:: ConfigurationManager(){
  EEPROM.begin(PARAMS_MESSAGE_SIZE);
}

ParamsMessage ConfigurationManager::load(){
  ParamsMessage controlMsg;
  controlMsg.isOn = EEPROM.readByte(CONFIG_IS_ON_BYTE);
  controlMsg.targetTemp = EEPROM.readByte(CONFIG_TEMPERATURE_BYTE);
  controlMsg.power = EEPROM.readByte(CONFIG_POWER_BYTE);
  controlMsg.isOnSchedule = EEPROM.readByte(CONFIG_IS_ON_SCHEDULE_BYTE);
  controlMsg.scheduleRaw = EEPROM.readString(CONFIG_SCHEDULE_BYTE);

  Serial.printf("ReadConfig %i %i %i %i\n", controlMsg.isOn, controlMsg.targetTemp, controlMsg.power, controlMsg.isOnSchedule);
  Serial.println(controlMsg.scheduleRaw);

  DynamicJsonDocument doc(PARAMS_MESSAGE_SIZE);
  deserializeJson(doc, controlMsg.scheduleRaw);
  schedule.parse(doc.as<JsonObject>());

  return controlMsg;
}

void ConfigurationManager::saveConfig(uint8_t receivedValue, uint8_t currentValue, uint8_t storage){
  if (receivedValue != 0 && receivedValue != currentValue){
    EEPROM.writeByte(storage, receivedValue);
    Serial.printf("Saved %i %i %i\n", receivedValue, currentValue, storage);
  }
}

void ConfigurationManager::save(ParamsMessage paramsMsg, ParamsMessage controlMsg){
  saveConfig(paramsMsg.power, controlMsg.power, CONFIG_POWER_BYTE);
  saveConfig(paramsMsg.targetTemp, controlMsg.targetTemp, CONFIG_TEMPERATURE_BYTE);
  saveConfig(paramsMsg.isOn, controlMsg.isOn, CONFIG_IS_ON_BYTE);
  saveConfig(paramsMsg.isOnSchedule, controlMsg.isOnSchedule, CONFIG_IS_ON_SCHEDULE_BYTE);
  if(paramsMsg.schedule.size() > 0){
    Serial.println(paramsMsg.scheduleRaw);
    EEPROM.writeString(CONFIG_SCHEDULE_BYTE, paramsMsg.scheduleRaw);
    Serial.println("Saved schedule");
  }
    
  EEPROM.commit();
}