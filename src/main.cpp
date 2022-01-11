#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Button2.h> 
#include <ESPRotary.h>
#include <max6675.h>
#include <Ticker.h>

byte currentTemp;
byte targetTemp;
byte power;
bool isCooling;

#define MAX_POWER  3
#define MIN_POWER  1
#define PIN_4KWT  1
#define PIN_8KWT  2
#define PIN_12KWT  3
byte powerPins [3]= {PIN_4KWT, PIN_8KWT, PIN_12KWT};


// screen properties
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D 
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// rotary encoder properties
#define ROTARY_PIN1 1//D1
#define ROTARY_PIN2 2 //D2
#define BUTTON_PIN  4 //D4
#define CLICKS_PER_STEP 4   // this number depends on your rotary encoder
#define MIN_POS         20
#define MAX_POS         80
#define START_POS       20
#define INCREMENT       3   // this number is the counter increment on each step
ESPRotary r;
Button2 b;

// termopair properties
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// timer
Ticker timer(stopCoolingOff, 3*60*1000, 0, MICROS_MICROS);

void setup() {
  // pin modes
  pinMode(13, OUTPUT); 

  // init serial
  Serial.begin (9600);
  delay(50);
  
  setDefaultParams(b);

  initRotaryEncoder();

  initDisplay();
}

void initDisplay(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);

  // Clear the buffer
  display.clearDisplay();
}

void initRotaryEncoder(){
  b.begin(BUTTON_PIN);
  b.setTapHandler(setPower);
  b.setLongClickHandler(setDefaultParams);
  r.begin(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS, START_POS, INCREMENT);
  r.setChangedHandler(manageTemperatureChange);
}

void loop() {
  timer.update();

  // read target temperature
  r.loop();
  b.loop();

  // read current temperature
  currentTemp = (byte)thermocouple.readCelsius();

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
        timer.start();
        digitalWrite(powerPin, LOW);
      }
    }
  }
  

  writeTextOnScreen();
  delay(500);
}

void stopCoolingOff(){
 isCooling = false;
 timer.stop();
}

void writeTextOnScreen(){
  display.clearDisplay();

  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);             
  display.printf(PSTR("C: %i; T: %i; P: %i"), currentTemp, targetTemp, power);

  display.display();
}

void setPower(Button2& btn){
  power++;
  if(power > MAX_POWER)
    power = MIN_POWER;
}

void setDefaultParams(Button2& btn){
  power = 2;
  targetTemp = 40;
}

void manageTemperatureChange(ESPRotary& r) {
  targetTemp = r.getPosition();
}