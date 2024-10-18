#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>

#include "../include/credentials.hpp"
#include "../include/thingDescription.hpp"

int pin = 2;
int buzzerPin = 23;
bool uploaded = false;

JsonDocument doc;

WebServer server(80); //Create an instance of the web server on port 80

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!. This ESP serves both as client to a Thing Directory and as a server. Created by: narcisoleedev");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void getThingDescription(){
  server.send(200, "text/plain", thingDescription);
}

void doSomething() {
  tone(buzzerPin, 1000);
  delay(1000);            
  noTone(buzzerPin); 
  server.send(200, "text/plain", "buzz...");
}

void isUpdated(){
  digitalWrite(pin, HIGH); //Turn the led to say that the stuff was uploaded
}

void setup() {
  pinMode(pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);

  deserializeJson(doc, thingDescription);

  WiFi.begin(credentials::ssid, credentials::password); //Connect to WiFi
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  //Serial.println("Local IP: " + WiFi.localIP());
  Serial.println("Connected to the WiFi network");

  if (!MDNS.begin("esp32")){ //Start mDNS service
    Serial.println("Error starting mDNS");
    return;
  }

  if (!MDNS.addService("http", "tcp", credentials::port)) {
    Serial.println("Failed to add mDNS service");
    return;
  }
  Serial.println("HTTP service added to mDNS");

  server.on("/", handleRoot); //Server routes 
  server.on("/getThingDescription", getThingDescription);
  server.on("/doSomething", doSomething);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {

  Serial.println(uploaded);
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    if(!uploaded){ //If the thing was uploaded or not
      int n = MDNS.queryService("_wot", "_tcp"); //Query the directory by DNS-SD and returns >=0 if found
      delay(2500);
      if (n == 0) { 
        Serial.println("No services were found");
      } else {
        Serial.println("Service found");
        Serial.println(String(n) + " service(s) found");
        for (int i = 0; i < n; i++) {
            Serial.print("Service Name: ");
            Serial.println(MDNS.hostname(i));
            Serial.print("IP Address: ");
            Serial.println(MDNS.IP(i));
            Serial.print("Port: ");
            Serial.println(MDNS.port(i));
        }

        String thingDescriptionId = doc["id"].as<String>();
        String serverPath = "http://" + MDNS.IP(0).toString() + ":" + MDNS.port(0) + "/things/" + thingDescriptionId;
        Serial.println(serverPath); //Debuggggg :)

        if (http.begin(serverPath)) {
          Serial.println("HTTP Begin Successful");
          http.addHeader("Content-Type", "application/json");
          http.setTimeout(5000);
          int httpResponseCode = http.PUT(thingDescription);  // Get HTTP response code
          if (httpResponseCode > 0) {
              Serial.println("HTTP Response code: " + String(httpResponseCode));
          } else {
               Serial.println("Error on HTTP request: " + http.errorToString(httpResponseCode));
          }
          http.end();
          uploaded = true;
          isUpdated();
          } else {
              Serial.println("HTTP Begin Failed");
          }
      }
    } else {
      server.handleClient(); //Handle client
    }

  } else {
    Serial.println("WiFi Disconnected"); 
  }
}