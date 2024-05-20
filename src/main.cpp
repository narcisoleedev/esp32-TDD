// Include the Arduino library
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>

#include "../include/credentials.hpp"

const int ledPin = 2;
const int lumSensor = 4;

const char* thingDescription = "";

bool gotResponse = false;

void setup() {

  //Pining
  pinMode(ledPin, OUTPUT);
  pinMode(lumSensor, INPUT);

  Serial.begin(9600);

  WiFi.begin(credentials::ssid, credentials::password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  //mDNS start configuration
  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
    return;
  }
  MDNS.addService("http", "tcp", credentials::port);
  //TO test this use this command: curl http://esp32.local/getValue
}

void blinkLed(){
    digitalWrite(ledPin, HIGH);
    delay(100);
}

void loop() {
  if(WiFi.status()==WL_CONNECTED){
      HTTPClient http;
      String serverPath = "http://192.168.0.8:8000/things";
      http.begin(serverPath.c_str());
      
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code:");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code:");
        Serial.println(httpResponseCode);
      }
      gotResponse = true;
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
  }
  blinkLed();
  delay(100);
}
