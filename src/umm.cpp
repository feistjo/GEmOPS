#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <SPIFFS.h>

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    // while(1) digitalWrite(LED_BUILTIN, millis() % 200 < 50 ? HIGH : LOW);
  }
}


