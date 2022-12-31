/*********
  Author- Venkatesh Pampana
  Complete project details at 
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <SDL_Arduino_INA3221.h>

const char* ssid     = "SSB_Mini_wifi";
const char* password = "123456789";

#define LIPO_BATTERY_CHANNEL 2
#define SOLAR_CELL_CHANNEL 1
#define OUTPUT_CHANNEL 3
//address and shunt resistor value
SDL_Arduino_INA3221 ina3221(0x41,0.02);
//SensirionI2CSht4x shtSensor;
// current temperature & humidity, updated in loop()
float temperature = 0.0;
float humidity = 0.0;

unsigned long previousMillis = 0; 
unsigned long interval = 1000;  
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
//Adafruit_SHT4x sht4xSensor = Adafruit_SHT4x();
//12v output switch control pin
const uint8 outSwitchPin = 2;
const uint8 anlogPin = 0;
//char errorMessage[256];
String switchState;
String statusmsg;
String getTemperature() {
  // uint16_t error;
  // error = shtSensor.measureMediumPrecision(temperature, humidity);
  //     if (error) {
  //       Serial.print("Error : ");
  //       errorToString(error, errorMessage, 256);
  //       Serial.println(errorMessage);
  //     }
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float temperature = 1.8 * bme.readTemperature() + 32;
  //Serial.println(temperature);
  return String(temperature);
}
  
String getHumidity() {
  //float humidity = bme.readHumidity();
  Serial.println(humidity);
  return String(humidity);
}

String getReadings() {

  float inAmps = ina3221.getCurrent_mA(SOLAR_CELL_CHANNEL)/1000;
  float batAmps = ina3221.getCurrent_mA(LIPO_BATTERY_CHANNEL)/1000;  //  + means the battery is charging, - that it is
  float loadAmps = ina3221.getCurrent_mA(OUTPUT_CHANNEL)/1000;
  float inVolts=ina3221.getBusVoltage_V(SOLAR_CELL_CHANNEL);
  float batVolts=ina3221.getBusVoltage_V(LIPO_BATTERY_CHANNEL);
  float loadVolts=ina3221.getBusVoltage_V(OUTPUT_CHANNEL);
  float inPow =inAmps*inVolts;
  float batPow =batAmps*batVolts;
  float LoadPow =loadAmps*loadVolts;
  float temperature =analogRead(anlogPin);
  //Serial.println(pressure);
  return String("{\"sa\":" +String(inAmps)+",\"sv\":"+inVolts+",\"sp\":"+inPow+",\"bv\":"+batVolts+",\"ba\":"+batAmps+
  ",\"bs\":"+batPow+",\"lv\":"+loadVolts+",\"la\":"+loadAmps+",\"lp\":"+LoadPow+",\"temp\":"+temperature+"}");

}
// Replaces placeholder with sensor values
String processor(const String& var){
  //Serial.println(var);
  if(var == "STATE"){
    if(digitalRead(outSwitchPin)){
      switchState = "ON";                    
    }
    else{
      switchState = "OFF";
    }
    Serial.print(switchState);
    return switchState;
  }
  return String();
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  Serial.println("LittleFS mounted successfully");


  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  //set  output switch pin mode
  pinMode(outSwitchPin, OUTPUT);
  digitalWrite(outSwitchPin, HIGH);
  ina3221.begin();

  Serial.println(ina3221.getManufID(),HEX);
      //initialize wire library (used for sht4x sensor)
  Wire.begin(5,4);
  //shtSensor.begin(Wire);
  //uint32_t serialNumber;
  //shtSensor.serialNumber(serialNumber);
 // Serial.println(serialNumber);
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

    // Route to load js file
  server.on("/jquery-3.3.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/jquery-3.3.1.min.js", "text/css");
  });

    // Route to load img file (not working)
  // server.on("/solarboxlogo_header", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(LittleFS, "/solarboxlogo_header.png", "image/png");
  // });

      // Route to load img file
  server.on("/ssb_dashboard_final_v1.svg", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/ssb_dashboard_final_v1.svg", "image/svg+xml");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(outSwitchPin, HIGH);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(outSwitchPin, LOW);    
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  //   server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send_P(200, "text/plain", getTemperature().c_str());
  // });
  
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });
  server.addHandler(&ws);
  // Start server
  server.begin();
}

void loop() {

unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
  ws.textAll(getReadings());
  ws.cleanupClients();

  } 
}