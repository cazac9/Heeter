#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Button2.h> 
#include <ESPRotary.h>
#include <max6675.h>
#include <Ticker.h>

#define SDA_PIN 22
#define SCL_PIN 23

byte currentTemp;
byte targetTemp;
byte power;
// bool isCooling;

#define MAX_POWER  3
#define MIN_POWER  1
// #define PIN_4KWT  1
// #define PIN_8KWT  2
// #define PIN_12KWT  3
// byte powerPins [3]= {PIN_4KWT, PIN_8KWT, PIN_12KWT};


// screen properties
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initDisplay(){
  Serial.println(F("Start initing display"));
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

void writeTextOnScreen(){
  display.clearDisplay();
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);             
  display.printf("C: %i; T: %i; P: %i", currentTemp, targetTemp, power);
  display.display();
}

// rotary encoder properties
#define ROTARY_PIN1 12
#define ROTARY_PIN2 13 
#define BUTTON_PIN  14 
#define CLICKS_PER_STEP 4   
#define MIN_POS         20
#define MAX_POS         80
#define START_POS       20
#define INCREMENT       3  
ESPRotary r;
Button2 b;

void setPower(Button2& btn){
  Serial.println("Pressed set power");
  power++;
  if(power > MAX_POWER)
    power = MIN_POWER;
}

void setDefaultParams(Button2& btn){
  Serial.println("Pressed set default params");
  power = 2;
  targetTemp = 40;
}

void manageTemperatureChange(ESPRotary& r) {
  Serial.println("Changed target temperature value");
  targetTemp = r.getPosition();
}

void initRotaryEncoder(){
  Serial.println("Start initing rotary encoder");
  b.begin(BUTTON_PIN);
  b.setTapHandler(setPower);
  b.setLongClickHandler(setDefaultParams);
  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
  r.setChangedHandler(manageTemperatureChange);
}

// termopair properties
#define TERMO_SCLK 32
#define TERMO_CS 33
#define TERMO_SO 25
MAX6675 thermocouple(TERMO_SCLK, TERMO_CS, TERMO_SO);

// // timer
// Ticker timer(stopCoolingOff, 3*60*1000, 0, MICROS_MICROS);


void setup() {
  Serial.begin (115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  initDisplay();
  initRotaryEncoder();
  setDefaultParams(b);
}





void loop() {
  // timer.update();

  // read target temperature
  r.loop();
  b.loop();

  // read current temperature
  currentTemp = (byte)thermocouple.readCelsius();
  Serial.println(thermocouple.readCelsius());
  delay(500);

  // if(!isCooling){
  //   for (byte i = 0; i < power; i++)
  //   {
  //     byte powerPin = powerPins[i];
  //     if (currentTemp < targetTemp){
  //       // start heating
  //       digitalWrite(powerPin, HIGH);
  //     }
  //     else{
  //       // wait for coolling
  //       isCooling = true;
  //       timer.start();
  //       digitalWrite(powerPin, LOW);
  //     }
  //   }
  // }
  

  writeTextOnScreen();
  // delay(500);
}

// void stopCoolingOff(){
//  isCooling = false;
//  timer.stop();
// }