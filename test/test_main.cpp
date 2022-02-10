#include <Arduino.h>
#include <unity.h>
#include <messaging/ParamsMessage.h>
#include <HttpApiManager.h>
#include <Globals.h>
#include <ArduinoJson.h>

void setup() {
    UNITY_BEGIN();   
}

void test_json_parsing(void){
  ParamsMessage msg;
  const char * json = "{\"power\":2,\"current\":21,\"relay1state\":0,\"isOn\":2,\"target\":45,\"schedule\":{\"0\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"1\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"2\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"3\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"4\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"5\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"6\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}],\"7\":[{\"s\":0,\"e\":6,\"t\":8,\"p\":2},{\"s\":6,\"e\":9,\"t\":18,\"p\":2},{\"s\":9,\"e\":17,\"t\":12,\"p\":2},{\"s\":17,\"e\":22,\"t\":18,\"p\":2},{\"s\":22,\"e\":24,\"t\":8,\"p\":2}]},\"isOnSchedule\":2}";
  
  DynamicJsonDocument doc(512);
  deserializeJson(doc, (const char*)json);
  JsonObject object = doc.as<JsonObject>();
  msg.power = object["power"];
  msg.targetTemp = object["target"];
  msg.isOn = object["isOn"];
  msg.isOnSchedule = object["isOnSchedule"];
  msg.scheduleRaw = object["schedule"];
  msg.command = PARAMS;

  msg.parseSchedule(object["schedule"]);
  TEST_ASSERT_EQUAL(5, msg.schedule.size());
}

void loop() {

  ParamsMessage msg;
  RUN_TEST(test_json_parsing);
  UNITY_END();
}