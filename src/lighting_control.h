#pragma once
#include <Arduino.h>

enum LightMode { MODE_AUTO, MODE_MANUAL, MODE_PROJECTOR };

void initLighting();
void readSensors();
void applyLightingControl();

// أوامر من MQTT
void setMode(LightMode mode);
void setRelay1(bool on);
void setRelay2(bool on);
void setBrightness(int percent);  // 0-100
void setAllOff();
void setProjectorBrightness(int percent);  // 0-100, للسلايدر


// Getters
LightMode getCurrentMode();
bool      getRelay1State();
bool      getRelay2State();
int       getBrightnessPercent();
float     getLux();
bool      isPersonDetected();
int  getProjectorBrightnessPercent();
