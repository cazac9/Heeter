#include <max6675.h>
#include <TermocoupleManager.h>
#include <messaging/ParamsMessage.h>
#include <Globals.h>

void TermocoupleManager::runTask(void *pvParam){
  MAX6675 thermocouple(SCK, SS, MISO);
  ParamsMessage params;
  while (true)
  {
    params.currentTemp = (byte)thermocouple.readCelsius();
    params.command = PARAMS;
    xQueueOverwrite((QueueHandle_t)pvParam, &params);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}