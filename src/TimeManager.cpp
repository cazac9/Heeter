#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeManager.h>
#include <messaging/ParamsMessage.h>

#define SSID ""
#define PASSWORD ""

void TimeManager::runTask(void *pvParam){
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
  timeClient.begin();
  timeClient.setTimeOffset(7200);
  ParamsMessage params;
  params.command = DATE_TIME;

  while (true)
  {
    while (WiFi.status() == WL_CONNECTED) {
      while(!timeClient.update()) {
        timeClient.forceUpdate();
      }

      params.dateTime = timeClient.getFormattedTime();
      xQueueSend((QueueHandle_t)pvParam, &params, portMAX_DELAY);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}