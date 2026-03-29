#pragma once
#include <Arduino.h>

void initMQTT();
void mqttLoop();
void sendLightingData();