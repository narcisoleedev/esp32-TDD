// Include the Arduino library
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include "../include/credentials.hpp"

const int ledPin = 2;
const int lumSensor = 4;

bool gotResponse = false;

WebServer server(80); //Create an instance of the web server on port 80

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP32!");
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void thingDescription(){
  server.send(200, "text/plain", credentials::thingDescription);
}

void handleGetValue() {
  int lumValue = analogRead(lumSensor);
  String response = "Luminance Sensor Value: " + String(lumValue);
  server.send(200, "text/plain", response);
}

void setup() { ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(ledPin, OUTPUT);
  pinMode(lumSensor, INPUT);

  Serial.begin(9600);

  // Connect to Wi-Fi
  WiFi.begin(credentials::ssid, credentials::password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println(WiFi.localIP());

  // Start mDNS service
  if (!MDNS.begin("esp32")) {
    Serial.println("Error starting mDNS");
    return;
  }
  MDNS.addService("http", "tcp", credentials::port);
  Serial.println("mDNS responder started");

  if (!MDNS.addService("http", "tcp", credentials::port)) {
    Serial.println("Failed to add mDNS service");
    return;
  }
  Serial.println("HTTP service added to mDNS");

  //Server routes
  server.on("/", handleRoot);
  server.on("/getTD", handleGetValue);
  server.on("/getValue", handleGetValue);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  //browseService()
}

void blinkLed() {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  server.handleClient();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String serverPath = "http://192.168.0.8:8000/things";
    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code:");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      //Serial.println(payload);
    } else {
      Serial.print("Error code:");
      //Serial.println(httpResponseCode);
    }
    gotResponse = true;
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  blinkLed();
}