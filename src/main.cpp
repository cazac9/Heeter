#include <Arduino.h>
#include <messaging/ParamsMessage.h>
#include <HeaterManager.h>
#include <TermocoupleManager.h>
#include <DisplayManager.h>
#include <EncoderManager.h>
#include <Globals.h>
//todo: encoder more than max
// do something with innertion
// http
// check if water is moving
// android app

QueueHandle_t displayQ;
QueueHandle_t heatersQ;
QueueHandle_t inputQ;

TaskHandle_t display;
TaskHandle_t termocouple;
TaskHandle_t heaters;
TaskHandle_t encoder;

ParamsMessage controlMsg;

void halt(const char *msg){
  Serial.println(msg);
  Serial.flush();
  esp_deep_sleep_start();
}

byte calculatePower(){
  byte power = controlMsg.power + 1;
  if(power > MAX_POWER)
    power = MIN_POWER;
  
  return power;
}

void setDefaultParams(){
  controlMsg.power = 2;
  controlMsg.targetTemp = 27;
}

void setup() {
  Serial.begin (115200);
  inputQ = xQueueCreate(32, sizeof(ParamsMessage));
  if(!inputQ) 
    halt("Error creating input queue");
  
  displayQ = xQueueCreate(32, sizeof(ParamsMessage));
  if(!displayQ) 
    halt("Error creating display queue");

  heatersQ = xQueueCreate(32, sizeof(ParamsMessage));
  if(!heatersQ) 
  halt("Error creating encoder queue");

  if(xTaskCreate(DisplayManager::runTask, "display", 10000, displayQ, 1, &display) != pdPASS)
    halt("Erorr creating display task");

  if(xTaskCreate(HeaterManager::runTask, "heaters", 1024, heatersQ, 1, &heaters) != pdPASS)
    halt("Erorr creating heaters task");

  if(xTaskCreate(TermocoupleManager::runTask, "termocouple", 1024, inputQ, 1, &termocouple) != pdPASS)
    halt("Erorr creating termocouple task");

  if(xTaskCreate(EncoderManager::runTask, "encoder", 1024, inputQ, 1, &encoder) != pdPASS)
    halt("Erorr creating encoder task");

  setDefaultParams();
}

void loop() {
  ParamsMessage paramsMsg;
  if(xQueueReceive(inputQ, &paramsMsg, portMAX_DELAY) == pdTRUE){
    switch (paramsMsg.command)
    {
      case CT_SET:
        controlMsg.currentTemp = paramsMsg.currentTemp;
        break;
      case TT_SET:
        controlMsg.targetTemp = paramsMsg.targetTemp;
        break;
      case POWER_UP:
        controlMsg.power = calculatePower();
        break;
      case DEFAULTS:
        setDefaultParams();
        break;
      default:
        break;
    }

    xQueueSend(displayQ, &controlMsg, portMAX_DELAY);
    xQueueSend(heatersQ, &controlMsg, portMAX_DELAY);
  }
}