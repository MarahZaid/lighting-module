#include "mqtt_client.h"
#include "lighting_control.h"
#include "config.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

static WiFiClient   wifiClient;
static PubSubClient mqttClient(wifiClient);

// ─── استقبال أوامر من الـ Raspberry ──────────────
static void onMessage(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  Serial.print("MQTT IN: "); Serial.println(msg);

  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, msg)) return;

  // وضع التشغيل
  if (doc.containsKey("mode")) {
    String mode = doc["mode"].as<String>();
    if      (mode == "auto")      setMode(MODE_AUTO);
    else if (mode == "manual")    setMode(MODE_MANUAL);
    else if (mode == "projector") setMode(MODE_PROJECTOR);
  }

  // تحكم في الريليهات
  if (doc.containsKey("relay1"))
    setRelay1(doc["relay1"].as<bool>());

  if (doc.containsKey("relay2"))
    setRelay2(doc["relay2"].as<bool>());

  // شدة الإضاءة
  if (doc.containsKey("brightness"))
    setBrightness(doc["brightness"].as<int>());

  // إطفاء كامل
  if (doc.containsKey("allOff") && doc["allOff"].as<bool>())
    setAllOff();


  // سلايدر Projector Mode
  if (doc.containsKey("projector_brightness"))
    setProjectorBrightness(doc["projector_brightness"].as<int>());
    
}

// ─── إعادة الاتصال ────────────────────────────────
static void reconnect() {
  int tries = 0;
  while (!mqttClient.connected() && tries < 5) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected!");
      mqttClient.subscribe(TOPIC_LIGHTING_CMD);
    } else {
      Serial.print("failed rc="); Serial.println(mqttClient.state());
      delay(2000);
      tries++;
    }
  }
}

// ─── تهيئة ───────────────────────────────────────
void initMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(onMessage);
  mqttClient.setKeepAlive(30);
  reconnect();
}

// ─── Loop ─────────────────────────────────────────
void mqttLoop() {
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop();
}

// ─── بعث البيانات للـ Raspberry ──────────────────
void sendLightingData() {
  if (!mqttClient.connected()) return;

  StaticJsonDocument<256> doc;
  doc["roomId"]     = ROOM_ID;
  doc["mode"]       = getCurrentMode() == MODE_AUTO ? "auto" :
                      getCurrentMode() == MODE_MANUAL ? "manual" : "projector";
  doc["relay1"]     = getRelay1State();
  doc["relay2"]     = getRelay2State();
  doc["brightness"] = getBrightnessPercent();
  doc["lux"]        = getLux();
  doc["status"]     = (getRelay1State() || getRelay2State() ||
                       getBrightnessPercent() > 0) ? "on" : "off";
  doc["occupied"]   = isPersonDetected();
  doc["projector_brightness"] = getProjectorBrightnessPercent();

  String msg;
  serializeJson(doc, msg);
  mqttClient.publish(TOPIC_LIGHTING_DATA, msg.c_str());
  Serial.print("Data sent: "); Serial.println(msg);
}