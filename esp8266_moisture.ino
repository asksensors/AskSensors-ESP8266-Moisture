/*
 * AskSensors IoT Platform. 
 * Description: Soil moisture monitoring using ESP8266 and the AskSensors IoT cloud.
 *  Author: https://asksensors.com, 2018 - 2019
 *  github: https://github.com/asksensors/AskSensors-ESP8266-Moisture
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

// user config: TODO
#define  MOISTURE_THRESHOLD     55   // moisture alert threshold
const char* wifi_ssid = "......................";             // SSID
const char* wifi_password = "......................";         // WIFI
const char* apiKeyIn = "......................";      // API KEY IN
const unsigned int writeInterval = 25000; // write interval (in ms)

// ASKSENSORS config.
String host = "http://api.asksensors.com";         // ASKSENSORS API host name

ESP8266WiFiMulti WiFiMulti;

int moisture_Pin= 0; // Soil Moisture Sensor input at Analog PIN A0
int moisture_value= 0, moisture_state = 0xFF;

void setup() {

  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Soil Moisture monitoring using ESP8266 and AskSensors IoT cloud");
  Serial.println("Wait for WiFi... ");
  Serial.print("********** connecting to WIFI : ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("-> WiFi connected");
  Serial.println("-> IP address: ");
  Serial.println(WiFi.localIP());
 
}


void loop() {


    Serial.print("MOISTURE LEVEL : ");
    moisture_value= analogRead(moisture_Pin);
    moisture_value= moisture_value/10;
    Serial.println(moisture_value);
   if(moisture_value > MOISTURE_THRESHOLD) moisture_state = 0;
   else moisture_state = 1;
   

    // wait for WiFi connection
  if (WiFi.status() == WL_CONNECTED){

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        
        // Create a URL for the request
        String url = "";
        url += host;
        url += "/write/";
        url += apiKeyIn;
        url += "?module1=";
        url += moisture_value;
        url += "&module2=";
        url += moisture_state;
        
        Serial.print("********** requesting URL: ");
        Serial.println(url);
        http.begin(url); //HTTP
        
        Serial.println("> Soil moisture level and state were sent to ASKSENSORS");

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();

        Serial.println("********** End ");
        Serial.println("*****************************************************");
    }

    delay(writeInterval);
}
