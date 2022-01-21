#include <Arduino.h>
#include <messaging/ParamsMessage.h>
#include <HeaterManager.h>
#include <TermocoupleManager.h>
#include <DisplayManager.h>
#include <EncoderManager.h>
#include <Globals.h>
#include <WifiMonitor.h>
#include <esp_sntp.h>
#include <time.h>

//todo:
// do something with innertion
// http
// check if water is moving
// android app

// introduce schedule 
// ota

QueueHandle_t displayQ;
QueueHandle_t heatersQ;
QueueHandle_t inputQ;

TaskHandle_t display;
TaskHandle_t termocouple;
TaskHandle_t heaters;
TaskHandle_t encoder;
TaskHandle_t wifi;

ParamsMessage controlMsg;

void halt(const char *msg, const char *param){
  Serial.printf(msg);
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
  controlMsg.targetTemp = DEFAULT_TEMP;
}

QueueHandle_t createQueue(const char * name){
  QueueHandle_t queue = xQueueCreate(32, sizeof(ParamsMessage));
  if(!inputQ) 
    halt("Error creating %s queue", name);

  return queue;
}

void createTask(TaskFunction_t task, const char * name, QueueHandle_t q, TaskHandle_t handle, int stack = 1024){
  if(xTaskCreate(task, name, stack, q, 1, &handle) != pdPASS)
    halt("Erorr creating %s task", name);
}

void configureTime(){
  sntp_setoperatingmode(SNTP_OPMODE_POLL);
  sntp_setservername(0, "ua.pool.ntp.org");
  sntp_setservername(1, "time.google.com");
  sntp_setservername(2, "pl.pool.ntp.org");
  sntp_init();

  time_t now;
  char strftime_buf[64];
  struct tm timeinfo;

  time(&now);
  setenv("TZ", "GMT+2", 1);
  tzset();

  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
}

void setup() {
  Serial.begin (115200);
  inputQ = createQueue("input");
  displayQ = createQueue("display");
  heatersQ = createQueue("heaters");

  createTask(DisplayManager::runTask, "display", displayQ, &display, 10000);
  createTask(HeaterManager::runTask, "heaters", heatersQ, &heaters);
  createTask(TermocoupleManager::runTask, "termocouple", inputQ, &termocouple);
  createTask(EncoderManager::runTask, "encoder", inputQ, &encoder);
  createTask(WifiMonitor::runTask, "monitor", NULL, &wifi);

  setDefaultParams();
  configureTime();
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
      case DATE_TIME:
        controlMsg.dateTime = paramsMsg.dateTime;
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