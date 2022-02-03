#include <messaging/ParamsMessage.h>
#include <Globals.h>
#include <HttpApiManager.h>
#include <AsyncTCP.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncElegantOTA.h>

ParamsMessage msg;
AsyncWebServer server(80);

void HttpApiManager::runTask(void *pvParam){
  QueueHandle_t* queues = (QueueHandle_t *) pvParam;
  QueueHandle_t input = queues[0];
  QueueHandle_t http = queues[1];
  free(pvParam);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  server.on("/getCurentState", HTTP_GET, [](AsyncWebServerRequest *request){
    StaticJsonDocument<100> doc;
    JsonObject obj = doc.to<JsonObject>();
    obj["target"] = msg.targetTemp;
    obj["current"] = msg.currentTemp;
    obj["power"] = msg.power;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  AsyncCallbackJsonWebHandler* handler = new AsyncCallbackJsonWebHandler("/postSettings", [input](AsyncWebServerRequest *request, JsonVariant &json) {
    JsonObject object = json.as<JsonObject>();
    msg.power = object["power"];
    msg.targetTemp = object["target"];
    msg.command = PARAMS;
    xQueueOverwrite(input, &msg);

    request->send(200);
  });

  AsyncElegantOTA.begin(&server, OTA_USER, PASSWORD); 
  server.addHandler(handler);
  server.begin();


  while (true){
    xQueueReceive(http, &msg, portMAX_DELAY);
   }
}