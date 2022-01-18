#include <ESPRotary.h>
#include <Button2.h> 
#include <Arduino.h>
#include <EncoderManager.h>
#include <Globals.h>
#include <messaging/ParamsMessage.h>

QueueHandle_t encoderQ;
ParamsMessage params;

#define MIN_POS         20
#define MAX_POS         80
#define START_POS       20
void setPowerCallback(Button2& btn){
  Serial.println("Pressed set power");
  params.command = POWER_UP;
  xQueueSend(encoderQ, &params, portMAX_DELAY);
}

void setDefaultParamsCallback(Button2& btn){
  Serial.println("Pressed set default params");
  params.command = DEFAULTS;
  xQueueSend(encoderQ, &params, portMAX_DELAY);
}

void manageTemperatureChange(ESPRotary& r) {
  Serial.println("Changed target temperature value");
  params.command = TT_SET;
  params.targetTemp = r.getPosition();
  xQueueSend(encoderQ, &params, portMAX_DELAY);
}

void EncoderManager::runTask(void *pvParam){
  encoderQ = (QueueHandle_t)pvParam;

  Serial.println("Start initing rotary encoder");
  ESPRotary r;
  Button2 b;
  b.begin(14);
  b.setTapHandler(setPowerCallback);
  b.setLongClickHandler(setDefaultParamsCallback);
  r.begin(12, 13, 4, MIN_POS, MAX_POS, START_POS, 3);
  r.setChangedHandler(manageTemperatureChange);
  while (true)
  {
      r.loop();
      b.loop();
  }
}