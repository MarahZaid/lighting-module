#pragma once
#include <IPAddress.h>

// ─── WiFi ────────────────────────────────────────
#define WIFI_SSID           "nadia"
#define WIFI_PASSWORD       "12345678"

// ─── Network ─────────────────────────────────────
#define IP_THIS_ESP32       192,168,2,103
#define IP_GATEWAY          192,168,2,1
#define IP_SUBNET           255,255,255,0
#define IP_DNS              192,168,2,1

// ─── MQTT ────────────────────────────────────────
#define MQTT_BROKER         "192.168.2.101"
#define MQTT_PORT           1883
#define MQTT_CLIENT_ID      "esp32-lighting"

// ─── MQTT Topics ─────────────────────────────────
#define TOPIC_LIGHTING_DATA    "room/Room101/lighting/data"
#define TOPIC_LIGHTING_CMD     "room/Room101/lighting/command"

// ─── Room ─────────────────────────────────────────
#define ROOM_ID             "Room101"

// ─── Hardware Pins ────────────────────────────────
#define PIR_PIN             34
#define RADAR_PIN           27
#define PWM_PIN             32
#define RELAY1_PIN          33   // ضو أمامي
#define RELAY2_PIN          26   // ضو خلفي

// ─── PWM Settings ────────────────────────────────
#define PWM_FREQ            500
#define PWM_RESOLUTION      8    // 0-255

// ─── Auto Mode Settings ──────────────────────────
#define OFF_DELAY           15000  // 15 ثانية بدون حركة
#define LUX_MAX             800    // أقصى قراءة Lux
//#define PROJECTOR_BRIGHTNESS 51    // 20% في وضع Projector

// ─── Intervals ───────────────────────────────────
#define SEND_INTERVAL       5000   // بعث بيانات كل 5 ثواني
#define READ_INTERVAL       500    // قراءة sensors كل 0.5 ثانية