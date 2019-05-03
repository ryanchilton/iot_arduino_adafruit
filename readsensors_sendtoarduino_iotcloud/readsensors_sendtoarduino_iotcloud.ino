#include "arduino_secrets.h"
/* 
   Sketch generated by the Arduino IoT Cloud Thing "Environmental_Sensors"
   https://create.arduino.cc/cloud/things/c8d546ba-c356-42af-91d7-4b9e4c0e80a7 

  Arduino IoT Cloud Properties description

  The following variables are automatically generated and updated when changes are made to the Thing properties

  float HUMIDITY;
  float TEMPERATURE;
  float PRESSURE;
  float LIGHT_INTENSITY;

  Properties which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "thingProperties.h"
#include <avr/dtostrf.h>
#include <SPI.h>

////////////////////////////////
#include <Arduino_MKRENV.h>

////////////////////////////////
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735

#define TFT_CS        3
#define TFT_RST       2 
#define TFT_DC        1

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST); 
///////////////////////////
float _temperature = 0;
float _humidity = 0;
float _pressure = 0;
float _lux = 0;

unsigned long lastConnectionTime = 0;              // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 2000;       // delay between updates, in milliseconds

int debug = 0;
void setup() {
  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500); 

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
    // Init ST7735R chip, green tab
  tft.initR(INITR_144GREENTAB); 

  tft.fillScreen(ST77XX_BLACK);
  
   if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
    while (1);
  }
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

}

void loop() {

  // if 2 seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) 
  {
    // Your code here 
    readSensors();
    
    TEMPERATURE = _temperature;
    HUMIDITY = _humidity;
    PRESSURE = _pressure;
    LIGHT_INTENSITY = _lux;
   
    displayValuesOnTFT();
     
    lastConnectionTime = millis();

   /*
     NOTE: We put "ArduinoCloud.update()" inside the "if condition" only to make sure that the values shown in the TFT 
     are the same as those shown in the cloud in the same time interval. But this function by itself is responsible for 
     sending every 2 seconds the value of the variables to the cloud.
    */
    ArduinoCloud.update(); 
    
    testdrawtext(5,120,"data sent to Cloud!", ST77XX_WHITE,1);
    
  }
  
}

//Read sensors value: Temperature, Humidity, Pressure, Lux
void readSensors()
{
  _temperature = ENV.readTemperature();
  _humidity = ENV.readHumidity();
  _pressure = ENV.readPressure();
  _lux = ENV.readLux();
}


// Display values on TFT
void displayValuesOnTFT()
{

  // float to char conversion
  char temperature[6];
  char humidity[6];
  char pressure[7];
  char lux[8];
  
  dtostrf(_temperature, 5, 2, temperature);
  dtostrf(_humidity, 5, 2, humidity);
  dtostrf(_pressure, 5, 2, pressure);
  dtostrf(_lux, 6, 2, lux);
  
  // Display on TFT
  tft.fillScreen(ST77XX_BLACK);
  
  testdrawtext(0,5,"T:", ST77XX_RED,2);
  testdrawtext(25,5,temperature, ST77XX_RED,2);
  testdrawtext(110,10,"C", ST77XX_RED,1);
  
  testdrawtext(0,35,"H:", ST77XX_GREEN,2);
  testdrawtext(25,35,humidity, ST77XX_GREEN,2);
  testdrawtext(110,40,"%", ST77XX_GREEN,1);
  
  testdrawtext(0,65,"P:", ST77XX_YELLOW,2);
  testdrawtext(25,65,pressure, ST77XX_YELLOW,2);
  testdrawtext(110,70,"kPa", ST77XX_YELLOW,1);
  
  testdrawtext(0,95,"L:", ST77XX_WHITE,2);
  testdrawtext(25,95,lux, ST77XX_WHITE,2);
  testdrawtext(110,100,"Lux", ST77XX_WHITE,1);
 
}

void testdrawtext(int x, int y, char *text, uint16_t color, int size) {
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.setTextSize(size);
  tft.print(text);
}