#include <WiFi.h>
#include <tasks/WifiMonitor.h>
#include <esp_sntp.h>
#include <time.h>
#include <Globals.h>

_VOID      _EXFUN(tzset,	(_VOID));
int	_EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));

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
  WiFi.begin(WIFI_SSID, PASSWORD);
  WiFi.setAutoReconnect(true);
  WiFi.setHostname("heater");
  pinMode(LED_BUILTIN, OUTPUT);
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }

  Serial.println("\nconnected to wifi");
  Serial.println(WiFi.localIP().toString());
  
  digitalWrite(LED_BUILTIN, HIGH);

  configureTime();
  vTaskDelete(NULL);
}