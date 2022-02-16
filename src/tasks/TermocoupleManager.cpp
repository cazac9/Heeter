#include <max6675.h>
#include <tasks/TermocoupleManager.h>
#include <Globals.h>

#define T_SCK 5
#define T_SS 18

void TermocoupleManager::runTask(void *pvParam){
  
  MAX6675 thermocouple(T_SCK, T_SS, MISO);
  ParamsMessage params;
  while (true)
  {
    params.currentTemp = (byte)thermocouple.readCelsius();
    params.command = PARAMS;
    xQueueOverwrite((QueueHandle_t)pvParam, &params);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}