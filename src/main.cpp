#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WebServer.h>

#include "../include/credentials.hpp"
#include "../include/thingDescription.hpp"

bool uploaded = false;

WebServer server(80); //Create an instance of the web server

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!");
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

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(credentials::ssid, credentials::password);
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  // Start mDNS service
  if (!MDNS.begin("esp32")){
    Serial.println("Error starting mDNS");
    return;
  }
  if (!MDNS.addService("http", "tcp", credentials::port)) {
    Serial.println("Failed to add mDNS service");
    return;
  }
  Serial.println("HTTP service added to mDNS");

  //Server routes
  server.on("/", handleRoot);
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

    if(!uploaded){
      int n = MDNS.queryService("wot", "tcp");
      if (n == 0) {
        Serial.println("No services were found");
      } else {
        Serial.println("Service found");

        String serverPath = "http://" + MDNS.IP(n).toString() + ":8000/things/urn:dev:ops:32473-ESP32-001";
        Serial.println(serverPath);
        http.begin(serverPath.c_str());

        int httpResponseCode = http.PUT(thingDescription);
        Serial.println("HTTP Response code:" + httpResponseCode);

        String payload = http.getString();
        Serial.println(payload);

        http.end();
      }
    } else {
      server.handleClient();
    }

  } else {
    Serial.println("WiFi Disconnected");
  }
}