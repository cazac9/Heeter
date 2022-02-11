#include <tasks/HeaterManager.h>
#include <Globals.h>

#define WAIT_3MINS 3 * 60 * 1000
#define PIN_4KWT  32
#define PIN_8KWT  33
#define PIN_12KWT  25


void switchOff(byte powerPins[]){
  for (byte i = 0; i < MAX_POWER; i++)
    digitalWrite(powerPins[i], LOW);
}

void HeaterManager::runTask(void *pvParam){
  byte powerPins[3] = {PIN_4KWT, PIN_8KWT, PIN_12KWT};
  ParamsMessage msg;
  for (byte i = 0; i < MAX_POWER; i++){
    pinMode(powerPins[i], OUTPUT);
  }

  while (true)
  {
    if(xQueueReceive((QueueHandle_t)pvParam, &msg, portMAX_DELAY) == pdTRUE){
      if (msg.flow <= MIN_WATER_FLOW || msg.isOn != 1) 
      {
        switchOff(powerPins);
        continue;
      }

      if (msg.currentTemp < msg.targetTemp){
        for (byte i = 0; i < MAX_POWER; i++)
          digitalWrite(powerPins[i], i < msg.power ? HIGH : LOW);
      } else {
        switchOff(powerPins);
        vTaskDelay(WAIT_3MINS / portTICK_PERIOD_MS);
      }
    }

  }
}
