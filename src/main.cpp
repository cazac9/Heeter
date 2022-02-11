#include <Tasks.h>
#include <Globals.h>
#include <ArduinoJson.h>
#include <ConfigurationManager.h>

using namespace std;
//todo:
// do something with innertion
// android app
// introduce schedule 
// encoder is working weird

QueueHandle_t displayQ;
QueueHandle_t heatersQ;
QueueHandle_t inputQ;
QueueHandle_t httpQ;

ConfigurationManager config;

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

  controlMsg = config.load();

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
}

void manageSchedule(ParamsMessage target, ParamsMessage source){
  bool isOnSchedule = source.isOnSchedule == 1
     || (target.isOnSchedule == 1 && source.isOnSchedule != 2);
  
  if(!isOnSchedule)
    return;
  vector<vector<ScheduleRange>> schedule = source.schedule;
  if (schedule.size() == 0)
    schedule = target.schedule;
   
  struct tm timeinfo;
  if(schedule.size() > 0  && getLocalTime(&timeinfo)){
    target.schedule = schedule;
    float timenow = timeinfo.tm_hour + (float)(timeinfo.tm_min / 60);
    vector<ScheduleRange> daylySchedule  = schedule[timeinfo.tm_wday];
    for (size_t i = 0; i < 10; i++)
    {
      if(timenow >= daylySchedule[i].start && timenow <= daylySchedule[i].end){
        target.targetTemp = daylySchedule[i].targetTemp;
        target.power = daylySchedule[i].power;
        break;
      }
    }
  }
}

void loop() {
  ParamsMessage paramsMsg;
  if(xQueueReceive(inputQ, &paramsMsg, portMAX_DELAY) == pdTRUE){
    switch (paramsMsg.command)
    {
      case PARAMS:
        config.save(paramsMsg, controlMsg);

        //manageSchedule(controlMsg, paramsMsg);

        controlMsg.currentTemp = paramsMsg.currentTemp == 0 ? controlMsg.currentTemp : paramsMsg.currentTemp;
        controlMsg.targetTemp = paramsMsg.targetTemp == 0 ? controlMsg.targetTemp : paramsMsg.targetTemp;
        controlMsg.power = paramsMsg.power == 0 ? controlMsg.power : paramsMsg.power;
        controlMsg.flow = paramsMsg.flow == 0 ? controlMsg.flow : paramsMsg.flow;
        controlMsg.isOn = paramsMsg.isOn == 0 ? controlMsg.isOn : paramsMsg.isOn;
        controlMsg.scheduleRaw =  paramsMsg.schedule.size() > 0 ? paramsMsg.scheduleRaw : controlMsg.scheduleRaw;
        controlMsg.isOnSchedule = paramsMsg.isOnSchedule == 0 ? controlMsg.isOnSchedule : paramsMsg.isOnSchedule;

        break;
      case POWER_UP:
        {
          uint8_t power = controlMsg.power + 1;
          if(power > MAX_POWER)
            power = MIN_POWER;
          paramsMsg.power = power;

          config.save(paramsMsg, controlMsg);
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
