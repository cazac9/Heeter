#include <messaging/ParamsMessage.h>
#include <HeaterManager.h>
#include <TermocoupleManager.h>
#include <DisplayManager.h>
#include <EncoderManager.h>
#include <Globals.h>
#include <WifiMonitor.h>
#include <HttpApiManager.h>
#include <WaterFlowManager.h>
#include <EEPROM.h>

//todo:
// do something with innertion
// android app
// introduce schedule 
// encoder is working weird

QueueHandle_t displayQ;
QueueHandle_t heatersQ;
QueueHandle_t inputQ;
QueueHandle_t httpQ;

ParamsMessage controlMsg;

void halt(const char *msg, const char *param){
  Serial.printf(msg, param);
  Serial.flush();
  esp_deep_sleep_start();
}

QueueHandle_t createQueue(const char * name){
  QueueHandle_t queue = xQueueCreate(1, sizeof(ParamsMessage));
  if(!queue) 
    halt("Error creating %s queue", name);

  return queue;
}

void createTask(TaskFunction_t task, const char * name, QueueHandle_t q, TaskHandle_t handle, int stack = 1024, int core = CONFIG_ARDUINO_RUNNING_CORE){
  if(xTaskCreatePinnedToCore(task, name, stack, q, 1, &handle, core) != pdPASS)
    halt("Erorr creating %s task", name);
}

void setup() {
  TaskHandle_t display;
  TaskHandle_t termocouple;
  TaskHandle_t heaters;
  TaskHandle_t encoder;
  TaskHandle_t wifi;
  TaskHandle_t httpApi;
  TaskHandle_t waterflow;

  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);

  inputQ = createQueue("input");
  displayQ = createQueue("display");
  heatersQ = createQueue("heaters");
  httpQ = createQueue("http");

  QueueHandle_t* queues = (QueueHandle_t*)malloc(sizeof(QueueHandle_t)*2);
  queues[0] = inputQ;
  queues[1] = httpQ;
  createTask(DisplayManager::runTask, "display", displayQ, &display, 10 * 1024);
  createTask(HeaterManager::runTask, "heaters", heatersQ, &heaters);
  createTask(TermocoupleManager::runTask, "termocouple", inputQ, &termocouple, 1024, 0);
  createTask(EncoderManager::runTask, "encoder", inputQ, &encoder, 1024, 0);
  createTask(WaterFlowManager::runTask, "waterflow", inputQ, &waterflow, 1024, 0);
  createTask(HttpApiManager::runTask, "server", queues, &httpApi, 10 * 1024);
  createTask(WifiMonitor::runTask, "monitor", NULL, &wifi, 10 * 1024);

  controlMsg.isOn = EEPROM.readByte(CONFIG_IS_ON_BYTE);
  controlMsg.targetTemp = EEPROM.readByte(CONFIG_TEMPERATURE_BYTE);
  controlMsg.power = EEPROM.readByte(CONFIG_POWER_BYTE);
}

void loop() {
  ParamsMessage paramsMsg;
  if(xQueueReceive(inputQ, &paramsMsg, portMAX_DELAY) == pdTRUE){
    switch (paramsMsg.command)
    {
      case PARAMS:
        controlMsg.currentTemp = paramsMsg.currentTemp == 0 ? controlMsg.currentTemp : paramsMsg.currentTemp;
        controlMsg.targetTemp = paramsMsg.targetTemp == 0 ? controlMsg.targetTemp : paramsMsg.targetTemp;
        controlMsg.power = paramsMsg.power == 0 ? controlMsg.power : paramsMsg.power;
        controlMsg.flow = paramsMsg.flow == 0 ? controlMsg.flow : paramsMsg.flow;
        controlMsg.isOn = paramsMsg.isOn == 0 ? controlMsg.isOn : paramsMsg.isOn;
        break;
      case POWER_UP:
        {
          uint8_t power = controlMsg.power + 1;
          if(power > MAX_POWER)
            power = MIN_POWER;
          controlMsg.power = power;

          EEPROM.writeByte(CONFIG_POWER_BYTE, power);
          EEPROM.commit();
          break;
        }
      default:
        break;
    }

    xQueueOverwrite(displayQ, &controlMsg);
    xQueueOverwrite(heatersQ, &controlMsg);
    xQueueOverwrite(httpQ, &controlMsg);
  }
}
