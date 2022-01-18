#include <Arduino.h>
#include <max6675.h>
#include <TermocoupleManager.h>
#include <messaging/ParamsMessage.h>
#include <Globals.h>

void TermocoupleManager::runTask(void *pvParam){
  MAX6675 thermocouple(32, 33, 25);
  ParamsMessage params;
  while (true)
  {
    float temperature = thermocouple.readCelsius();
    Serial.println(temperature);
    params.currentTemp = (byte)temperature;
    params.command = CT_SET;
    xQueueSend((QueueHandle_t)pvParam, &params, portMAX_DELAY);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}