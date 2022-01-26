#include <messaging/ParamsMessage.h>
#include <Globals.h>
#include <HttpApiManageger.h>
#include <WebServer.h>
#include <ArduinoJson.h>

ParamsMessage msg;
WebServer server(80);
QueueHandle_t input;
QueueHandle_t http;

void getCurrentSettings(){
  StaticJsonDocument<100> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["target"] = msg.targetTemp;
  obj["current"] = msg.currentTemp;
  obj["power"] = msg.power;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void postSettings(){
  StaticJsonDocument<100> doc;
  String json = server.arg("plain");
  deserializeJson(doc, json);

  JsonObject object = doc.as<JsonObject>();
  msg.power = object["power"];
  msg.targetTemp = object["target"];
  msg.command = TT_POWER_SET;
  xQueueSend(input, &msg, portMAX_DELAY);

  server.send(200);
}

void HttpApiManageger::runTask(void *pvParam){
  QueueHandle_t* queues = (QueueHandle_t *) pvParam;
  input = queues[0];
  http = queues[1];
  free(pvParam);

  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  server.on("/getCurentState", HTTP_GET, getCurrentSettings);
  server.on("/postSettings", HTTP_POST, postSettings);
  server.begin();
  while (true){
    server.handleClient();
    xQueueReceive(http, &msg, portMAX_DELAY);
   }
}