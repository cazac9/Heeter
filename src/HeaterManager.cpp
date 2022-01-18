#include <Arduino.h>
#include <messaging/ParamsMessage.h>
#include <HeaterManager.h>
#include <Globals.h>

#define WAIT_3MINS 180000
#define PIN_4KWT  5
#define PIN_8KWT  18
#define PIN_12KWT  19
byte powerPins[3] = {PIN_4KWT, PIN_8KWT, PIN_12KWT};

void HeaterManager::runTask(void *pvParam){
  ParamsMessage msg;
  for (byte i = 0; i < MAX_POWER; i++){
    pinMode(powerPins[i], OUTPUT);
  }

  while (true)
  {
    Serial.println("Heater manager");
    if(xQueueReceive((QueueHandle_t)pvParam, &msg, portMAX_DELAY) == pdTRUE){
      Serial.println(msg.currentTemp );
      Serial.println(msg.targetTemp);
      if (msg.currentTemp < msg.targetTemp){
        Serial.println("Heater manager warming");
        for (byte i = 0; i < MAX_POWER; i++){
          digitalWrite(powerPins[i], i < msg.power ? HIGH : LOW);
          Serial.println(i < msg.power ? HIGH : LOW);
        }
      } else {
        Serial.println("Heater manager cooling");
        for (byte i = 0; i < MAX_POWER; i++){
          digitalWrite(powerPins[i], LOW);
          vTaskDelay(WAIT_3MINS / portTICK_PERIOD_MS);
        }
      }
    }

  }
}
