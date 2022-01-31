#include <messaging/ParamsMessage.h>
#include <HeaterManager.h>
#include <TermocoupleManager.h>
#include <DisplayManager.h>
#include <EncoderManager.h>
#include <Globals.h>
#include <WifiMonitor.h>
#include <HttpApiManageger.h>


//todo:
// do something with innertion
// check if water is moving
// android app
// introduce schedule 
// encoder is working weird

QueueHandle_t displayQ;
QueueHandle_t heatersQ;
QueueHandle_t inputQ;
QueueHandle_t httpQ;

TaskHandle_t display;
TaskHandle_t termocouple;
TaskHandle_t heaters;
TaskHandle_t encoder;
TaskHandle_t wifi;
TaskHandle_t httpApi;

ParamsMessage controlMsg;

void halt(const char *msg, const char *param){
  Serial.printf(msg, param);
  Serial.flush();
  esp_deep_sleep_start();
}

uint8_t calculatePower(){
  uint8_t power = controlMsg.power + 1;
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
  if(!queue) 
    halt("Error creating %s queue", name);

  return queue;
}

void createTask(TaskFunction_t task, const char * name, QueueHandle_t q, TaskHandle_t handle, int stack = 1024){
  if(xTaskCreatePinnedToCore(task, name, stack, q, 1, &handle, CONFIG_ARDUINO_RUNNING_CORE) != pdPASS)
    halt("Erorr creating %s task", name);
}

void setup() {
  Serial.begin (115200);

  inputQ = createQueue("input");
  displayQ = createQueue("display");
  heatersQ = createQueue("heaters");
  httpQ = createQueue("http");

  QueueHandle_t* queues = (QueueHandle_t*)malloc(sizeof(QueueHandle_t)*2);
  queues[0] = inputQ;
  queues[1] = httpQ;
  createTask(DisplayManager::runTask, "display", displayQ, &display, 10 * 1024);
  createTask(HeaterManager::runTask, "heaters", heatersQ, &heaters);
  createTask(TermocoupleManager::runTask, "termocouple", inputQ, &termocouple);
  createTask(EncoderManager::runTask, "encoder", inputQ, &encoder);
  createTask(HttpApiManageger::runTask, "server", queues, &httpApi, 10 * 1024);
  createTask(WifiMonitor::runTask, "monitor", NULL, &wifi, 10 * 1024);

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
      case TT_POWER_SET:
        controlMsg.targetTemp = paramsMsg.targetTemp;
        controlMsg.power = paramsMsg.power;
        break;
      case DEFAULTS:
        setDefaultParams();
        break;
      default:
        break;
    }

    xQueueSend(displayQ, &controlMsg, portMAX_DELAY);
    xQueueSend(heatersQ, &controlMsg, portMAX_DELAY);
    xQueueSend(httpQ, &controlMsg, portMAX_DELAY);
  }
}
