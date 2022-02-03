#include <messaging/ParamsMessage.h>
#include <WaterFlowManager.h>
#include <Globals.h>

#define SENSOR  35
#define SECOND 1000


volatile byte pulseCount;

void WaterFlowManager::runTask(void *pvParam){
  pinMode(SENSOR, INPUT_PULLUP);

  ParamsMessage params;
  long currentMillis;
  long previousMillis = 0;
  byte pulse1Sec = 0;
  pulseCount = 0;

  attachInterrupt(digitalPinToInterrupt(SENSOR), [](){ pulseCount++; }, FALLING);

  while (true)
  {
    currentMillis = millis();
    if (currentMillis - previousMillis > SECOND) {
      
      pulse1Sec = pulseCount;
      pulseCount = 0;

      params.flow = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / 4.5;
      previousMillis = millis();
      params.command = PARAMS;
      
      xQueueOverwrite((QueueHandle_t)pvParam, &params);
    }
  }
}