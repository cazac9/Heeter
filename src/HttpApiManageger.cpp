#include <messaging/ParamsMessage.h>
#include <Globals.h>
#include <HttpApiManageger.h>
#include <WebServer.h>

ParamsMessage msg;

void getCurrentSettings(){

}

void postSettings(){
  
}

void HttpApiManageger::runTask(void *pvParam){

  WebServer server(80);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  server.on("/getCurentState", HTTP_GET, getCurrentSettings);
  server.on("/postSettings", HTTP_POST, postSettings);
  server.begin();
  while (true){
    server.handleClient();
    xQueueReceive((QueueHandle_t)pvParam, &msg, portMAX_DELAY);
  }
}
