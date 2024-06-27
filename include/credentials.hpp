#pragma once

#include <iostream>

namespace credentials{
    const char* ssid = "Flamengo";
    const char* password =  "gabigol123";
    const int port = 3000;
    const char* thingDescription = R"(
    {
    "@context": [
        "https://www.w3.org/2019/wot/td/v1",
        {
        "@language": "en"
        }
    ],
    "title": "ESP32 Device",
    "id": "urn:dev:ops:32473-ESP32-001",
    "base": "http://esp32.local",
    "security": [
        {
        "scheme": "nosec"
        }
    ],
    "properties": {
        "luminance": {
        "type": "integer",
        "title": "Luminance Sensor Value",
        "description": "The current value of the luminance sensor.",
        "readOnly": true,
        "forms": [
            {
            "href": "/getValue",
            "contentType": "application/json",
            "op": "readproperty"
            }
        ]
        }
    },
    "actions": {
        "blinkLed": {
        "title": "Blink LED",
        "description": "Blinks the onboard LED.",
        "forms": [
            {
            "href": "/blink",
            "contentType": "application/json",
            "op": "invokeaction"
            }
        ]
        }
    },
    "events": {},
    "links": [
        {
        "rel": "properties",
        "href": "/properties"
        },
        {
        "rel": "actions",
        "href": "/actions"
        },
        {
        "rel": "events",
        "href": "/events"
        }
    ]
    }
    )";
}


