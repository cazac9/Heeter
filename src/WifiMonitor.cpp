#include <WiFi.h>
#include <WifiMonitor.h>
#include <esp_sntp.h>
#include <time.h>
#include <TelnetStream.h>
#include <credentials.h>


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
  WiFi.setAutoReconnect(true);
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }

  Serial.println("connected to wifi");
  configureTime();
  
  Serial.println(WiFi.localIP());

  TelnetStream.begin();
  
  Serial.println("started wifi task done");
  while (true)
  {
    switch (TelnetStream.read()) {
      case 'R':
        TelnetStream.stop();
        delay(100);
        ESP.restart();
        break;
      case 'C':
        TelnetStream.println("bye bye");
        TelnetStream.stop();
        break;
    }
  }
}