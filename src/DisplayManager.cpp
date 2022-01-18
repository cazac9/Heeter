#include <Arduino.h>
#include <DisplayManager.h>
#include <Adafruit_SSD1306.h>
#include <messaging/ParamsMessage.h>

void DisplayManager::runTask(void *pvParam){
  Wire.begin(22, 23);
  Adafruit_SSD1306 display(128, 64, &Wire, 64);
  Serial.println("Start initing display");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();

  ParamsMessage msg;
  while (true)
  {
    if (xQueueReceive((QueueHandle_t)pvParam, &msg, portMAX_DELAY) == pdTRUE)
    {
      display.clearDisplay();
      display.setTextSize(1);             
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);             
      display.printf("C: %i; T: %i; P: %i", msg.currentTemp, msg.targetTemp, msg.power);
      display.display();
    }
  }
}