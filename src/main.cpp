#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>

#include "../include/credentials.hpp"
#include "../include/thingDescription.hpp"

int pin = 2;
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
  //doSomething
}

void isUpdated(){
  digitalWrite(pin, HIGH); //Turn the led to say that the stuff was uploaded
}

void setup() {
  pinMode(pin, OUTPUT);
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
  server.handleClient();
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    if(!uploaded){ //If the thing was uploaded or not
      int n = MDNS.queryService("wot", "tcp"); //Query the directory by DNS-SD and returns >=0 if found
      if (n == 0) { 
        Serial.println("No services were found");
      } else {
        Serial.println("Service found");

        String thingDescriptionId = doc["id"].as<String>();
        String serverPath = "http://" + MDNS.IP(n).toString() + ":8000/things/" + thingDescriptionId;
        Serial.println(serverPath); //Debuggggg :)

        http.begin(serverPath.c_str()); //Start http communication

        int httpResponseCode = http.PUT(thingDescription); //Get HTTP response code
        Serial.println("HTTP Response code:" + httpResponseCode);

        String payload = http.getString(); //Get the payload
        Serial.println(payload);

        http.end();
        uploaded = true;
        isUpdated();
      }
    } else {
      server.handleClient(); //If already uploaded it will handle the server as normal 
    }

  } else {
    Serial.println("WiFi Disconnected"); 
  }
}