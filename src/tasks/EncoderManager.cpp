#include <ESPRotary.h>
#include <Button2.h> 
#include <tasks/EncoderManager.h>
#include <Globals.h>

QueueHandle_t encoderQ;
ParamsMessage params;

#define MIN_POS         20
#define MAX_POS         80
#define START_POS       DEFAULT_TEMP

void EncoderManager::runTask(void *pvParam){
  encoderQ = (QueueHandle_t)pvParam;
  ESPRotary r;
  Button2 b;

  b.begin(13);
  r.begin(12, 14, 1, MIN_POS, MAX_POS, START_POS, 2);
  b.setTapHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time < 1000)
    {
      params.command = POWER_UP;
      xQueueOverwrite(encoderQ, &params);
    }
  });

  b.setLongClickHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time > 1000)
    {
      params.command = PARAMS;
      params.targetTemp = DEFAULT_TEMP;
      params.power = DEFAULT_POWER;
      xQueueOverwrite(encoderQ, &params);
    }
  });

  r.setChangedHandler([](ESPRotary& r) {
    params.command = PARAMS;
    params.isOn = 1;
    params.targetTemp = (uint8_t)r.getPosition();
    xQueueOverwrite(encoderQ, &params);
  });

  r.setLowerOverflowHandler([](ESPRotary& r) {
    params.isOn = 2;
    xQueueOverwrite(encoderQ, &params);
  });

  while (true)
  {
      r.loop();
      b.loop();
      vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}