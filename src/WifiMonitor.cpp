#include <Arduino.h>
#include <WiFi.h>
#include <WifiMonitor.h>
#include <esp_sntp.h>
#include <time.h>

#define SSID "realme C21Y"
#define PASSWORD "sedgi38smeeaerk"

void Wifi_disconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("disconnect");
  WiFi.begin(SSID, PASSWORD);
}

void configureTime(){
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "ua.pool.ntp.org");
  sntp_setservername(1, "time.google.com");
  sntp_setservername(2, "pl.pool.ntp.org");
  sntp_init();

  setenv("TZ", "GMT-2", 1);
  tzset();
}


void WifiMonitor::runTask(void *pvParam){
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("connected to wifi");
  configureTime();
  

  WiFi.onEvent(Wifi_disconnected, SYSTEM_EVENT_AP_STADISCONNECTED);
  Serial.println("started wifi task done");
  while (true)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}