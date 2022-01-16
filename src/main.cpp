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
bool isCooling;

// init heaters
#define MAX_POWER  3
#define MIN_POWER  1
#define PIN_4KWT  5
#define PIN_8KWT  18
#define PIN_12KWT  19
byte powerPins [3]= {PIN_4KWT, PIN_8KWT, PIN_12KWT};

void initHeaters(){
  for (byte i = 0; i < MAX_POWER; i++){
    pinMode(powerPins[i], OUTPUT);
  }
}


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

void setPower(byte value){
  power = value;
  if(power > MAX_POWER)
    power = MIN_POWER;

  for (byte i = 0; i < MAX_POWER; i++)
    digitalWrite(powerPins[i], powerPins[i] <= power ? HIGH : LOW);
}

void setPowerCallback(Button2& btn){
  Serial.println("Pressed set power");
  setPower(power + 1);
}

void setDefaultParams(Button2& btn){
  Serial.println("Pressed set default params");
  setPower(2);
  targetTemp = 40;
}

void manageTemperatureChange(ESPRotary& r) {
  Serial.println("Changed target temperature value");
  targetTemp = r.getPosition();
}

void initRotaryEncoder(){
  Serial.println("Start initing rotary encoder");
  b.begin(BUTTON_PIN);
  b.setTapHandler(setPowerCallback);
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
  initHeaters();
}

void loop() {
  // timer.update();

  // read target temperature
  r.loop();
  b.loop();

  currentTemp = (byte)thermocouple.readCelsius();
  Serial.println(thermocouple.readCelsius());
  //delay(500);

  if(!isCooling){
    for (byte i = 0; i < power; i++)
    {
      byte powerPin = powerPins[i];
      if (currentTemp < targetTemp){
        // start heating
        digitalWrite(powerPin, HIGH);
      }
      else{
        // wait for coolling
        isCooling = true;
       // timer.start();
        digitalWrite(powerPin, LOW);
      }
    }
  }
  

  writeTextOnScreen();
}

// void stopCoolingOff(){
//  isCooling = false;
//  timer.stop();
// }