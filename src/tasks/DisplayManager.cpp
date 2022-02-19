#include <tasks/DisplayManager.h>
#include <Adafruit_SSD1306.h>
#include <Globals.h>

void DisplayManager::runTask(void *pvParam){
  Adafruit_SSD1306 display(128, 64, &Wire);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  display.stopscroll();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);

  ParamsMessage msg;
  while (true)
  {
    if (xQueueReceive((QueueHandle_t)pvParam, &msg, portMAX_DELAY) == pdTRUE)
    {
      const char * errorCode= msg.isOnSchedule == 1 ? "SCHEDULE" : "MANUAL";

      if (msg.flow <= MIN_WATER_FLOW){
        errorCode = "E_WTR_FLW";
      }

      if (msg.isOn != 1){
        errorCode = "POWER_OFF";
      }

      display.clearDisplay();
      display.setCursor(0,0);
      display.printf("C:%i  T:%i\n", msg.currentTemp, msg.targetTemp);
      display.printf("P: %i  F:%i\n", msg.power, msg.flow);
      display.printf("%s\n", errorCode);
      struct tm timeinfo;
      if(getLocalTime(&timeinfo)){
        display.print(&timeinfo, "%H:%M:%S");
      }
      display.display();
      vTaskDelay(10 / portTICK_PERIOD_MS);
    }
  }
}