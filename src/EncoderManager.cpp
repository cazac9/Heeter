#include <ESPRotary.h>
#include <Button2.h> 
#include <EncoderManager.h>
#include <Globals.h>
#include <EEPROM.h>
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
  r.begin(12, 13, 1, MIN_POS, MAX_POS, START_POS, 2);
  b.setTapHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time < 1000)
    {
      Serial.println("Pressed set power");
      params.command = POWER_UP;
      xQueueOverwrite(encoderQ, &params);
    }
  });

  b.setLongClickHandler([](Button2& btn){
    unsigned int time = btn.wasPressedFor();
    if (time > 1000)
    {
      Serial.println("Pressed set default params");
      params.command = PARAMS;
      params.targetTemp = DEFAULT_TEMP;
      params.power = DEFAULT_POWER;
      xQueueOverwrite(encoderQ, &params);

      EEPROM.writeByte(CONFIG_POWER_BYTE, DEFAULT_POWER);
      EEPROM.writeByte(CONFIG_TEMPERATURE_BYTE, DEFAULT_TEMP);
      EEPROM.commit();
    }
  });

  r.setChangedHandler([](ESPRotary& r) {
    int value = r.getPosition();
    Serial.println("Changed target temperature value:");
    Serial.println(value);

    params.command = PARAMS;
    params.targetTemp = value;
    xQueueOverwrite(encoderQ, &params);

    EEPROM.writeByte(CONFIG_TEMPERATURE_BYTE, value);
    EEPROM.commit();
  });

  while (true)
  {
      r.loop();
      b.loop();
      vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}