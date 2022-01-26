#include <Globals.h>

void logger(const char * message){
  Serial.println(message);
  TelnetStream.println(message);
}

void logger(String message){
  Serial.println(message);
  TelnetStream.println(message);
}