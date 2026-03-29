#include <Arduino.h>
#include "config.h"
#include "wifi_manager.h"
#include "lighting_control.h"
#include "mqtt_client.h"

unsigned long lastSendTime = 0;
unsigned long lastReadTime = 0;

void setup() {
  Serial.begin(115200);
  connectWiFi();
  initLighting();
  initMQTT();
  Serial.println("Lighting Module Ready!");
}

void loop() {
  mqttLoop();

  unsigned long now = millis();

  // قراءة الـ sensors كل 0.5 ثانية
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;
    readSensors();
    applyLightingControl();
  }

  // بعث البيانات كل 5 ثواني
  if (now - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = now;
    sendLightingData();
  }
}