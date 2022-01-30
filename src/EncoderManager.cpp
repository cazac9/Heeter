#include <ESPRotary.h>
#include <Button2.h> 
#include <EncoderManager.h>
#include <Globals.h>
#include <messaging/ParamsMessage.h>

QueueHandle_t encoderQ;
ParamsMessage params;

#define MIN_POS         20
#define MAX_POS         80
#define START_POS       DEFAULT_TEMP

void EncoderManager::runTask(void *pvParam){
  encoderQ = (QueueHandle_t)pvParam;

  ESPRotary r;
  Button2 b;

  b.begin(14);
  r.begin(12, 13, 4, MIN_POS, MAX_POS, START_POS, 3);
  b.setTapHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time < 1000)
    {
      logger("Pressed set power");
      params.command = POWER_UP;
      xQueueSend(encoderQ, &params, portMAX_DELAY);
    }
  });

  b.setLongClickHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time > 1000)
    {
      logger("Pressed set default params");
      params.command = DEFAULTS;
      xQueueSend(encoderQ, &params, portMAX_DELAY);
    }
  });

  r.setChangedHandler([](ESPRotary& r) {
    logger("Changed target temperature value");
    params.command = TT_SET;
    params.targetTemp = r.getPosition();
    xQueueSend(encoderQ, &params, portMAX_DELAY);
  });

  while (true)
  {
      r.loop();
      b.loop();
      vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}