#include <DisplayManager.h>
#include <Adafruit_SSD1306.h>

#include <messaging/ParamsMessage.h>

void DisplayManager::runTask(void *pvParam){
  Adafruit_SSD1306 display(128, 64, &Wire);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    logger("SSD1306 allocation failed");
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
      display.clearDisplay();
      display.setCursor(0,0);
      display.printf("Current %i\n", msg.currentTemp);
      display.printf("Target  %i\n", msg.targetTemp);
      display.printf("Power   %i\n", msg.power);
      struct tm timeinfo;
      if(getLocalTime(&timeinfo)){
        display.print(&timeinfo, "%H:%M:%S");
      }
      display.display();
      delay(2000);
    }
    logger("out of mesage");
  }
}