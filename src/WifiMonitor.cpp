#include <Arduino.h>
#include <WiFi.h>
#include <WifiMonitor.h>

#define SSID ""
#define PASSWORD ""

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  WiFi.begin(SSID, PASSWORD);
}

void WifiMonitor::runTask(void *pvParam){
  WiFi.mode(WIFI_AP_STA );
  WiFi.begin(SSID, PASSWORD);

  WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_AP_STADISCONNECTED);
  while (true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}