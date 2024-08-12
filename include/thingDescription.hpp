#pragma once

const char* thingDescription = R"(
{
  "@context": [
    "https://www.w3.org/2019/wot/td/v1",
    {
      "language": "en"
    }
  ],
  "title": "ESP32 Device",
  "id": "urn:dev:ops:32473-ESP32-001",
  "base": "http://esp32.local",
  "security": ["nosec_sc"],
  "properties": {
    "status": {
      "type": "string",
      "title": "Device Status",
      "description": "Returns a status message from the device.",
      "readOnly": true,
      "forms": [
        {
          "href": "/",
          "contentType": "text/plain",
          "op": "readproperty"
        }
      ]
    },
    "thingDescription": {
      "type": "string",
      "title": "Thing Description",
      "description": "Provides the Thing Description of the device.",
      "readOnly": true,
      "forms": [
        {
          "href": "/getThingDescription",
          "contentType": "application/json",
          "op": "readproperty"
        }
      ]
    }
  },
  "actions": {
    "doSomething": {
      "title": "Do Something",
      "description": "Executes the 'doSomething' action on the device.",
      "forms": [
        {
          "href": "/doSomething",
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
  ],
  "securityDefinitions": {
    "nosec_sc": {
      "scheme": "nosec"
    }
  }
}
)";
