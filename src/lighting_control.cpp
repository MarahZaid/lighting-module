#include "lighting_control.h"
#include "config.h"
#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

static LightMode currentMode      = MODE_AUTO;
static bool      relay1State      = false;
static bool      relay2State      = false;
static int       currentPWM       = 0;
static float     currentLux       = 0;
static bool      personDetected   = false;
static unsigned long lastMotionTime = 0;

// ─── تهيئة ───────────────────────────────────────
void initLighting() {
  pinMode(PIR_PIN,    INPUT);
  pinMode(RADAR_PIN,  INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);

  ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(PWM_PIN, 0);
  ledcWrite(PWM_PIN, 0);

  Wire.begin();
  lightMeter.begin();

  Serial.println("Lighting Control Initialized");
}

// ─── تطبيق PWM تدريجياً ──────────────────────────
static void smoothPWM(int target) {
  if (currentPWM < target) {
    for (int b = currentPWM; b <= target; b++) {
      ledcWrite(0, b);
      currentPWM = b;
      delay(3);
    }
  } else if (currentPWM > target) {
    for (int b = currentPWM; b >= target; b--) {
      ledcWrite(0, b);
      currentPWM = b;
      delay(3);
    }
  }
}

// ─── تشغيل/إطفاء الريليهات ───────────────────────
static void applyRelay1(bool on) {
  relay1State = on;
  digitalWrite(RELAY1_PIN, on ? LOW : HIGH);
}

static void applyRelay2(bool on) {
  relay2State = on;
  digitalWrite(RELAY2_PIN, on ? LOW : HIGH);
}

// ─── قراءة الـ Sensors ───────────────────────────+
void readSensors() {
  bool pir   = digitalRead(PIR_PIN)   == HIGH;
  bool radar = digitalRead(RADAR_PIN) == HIGH;
  personDetected = pir || radar;

  if (personDetected) lastMotionTime = millis();

  currentLux = lightMeter.readLightLevel();
}

// ─── تطبيق المنطق حسب الـ Mode ───────────────────
void applyLightingControl() {
  if (currentMode == MODE_MANUAL) return; // الـ Raspberry يتحكم

  if (currentMode == MODE_PROJECTOR) {
    // أطفي الأضواء الرئيسية وخفف الـ dimmer
    applyRelay1(false);
    applyRelay2(false);
    smoothPWM(PROJECTOR_BRIGHTNESS);
    return;
  }

  // ── AUTO MODE ──────────────────────────────────
  bool noMotion = !personDetected &&
                  (millis() - lastMotionTime > OFF_DELAY);

  if (noMotion) {
    // إطفاء تدريجي
    smoothPWM(0);
    applyRelay1(false);
    applyRelay2(false);
  } else if (personDetected) {
    // تشغيل الأضواء
    applyRelay1(true);
    applyRelay2(true);

    // ضبط الـ dimmer حسب الضوء الطبيعي
    int targetPercent = map((int)currentLux, 0, LUX_MAX, 100, 0);
    targetPercent = constrain(targetPercent, 0, 100);
    int targetPWM = map(targetPercent, 0, 100, 0, 255);
    smoothPWM(targetPWM);
  }
}

// ─── أوامر من MQTT ────────────────────────────────
void setMode(LightMode mode) {
  currentMode = mode;
  Serial.print("Mode: ");
  Serial.println(mode == MODE_AUTO ? "AUTO" :
                 mode == MODE_MANUAL ? "MANUAL" : "PROJECTOR");
}

void setRelay1(bool on) {
  currentMode = MODE_MANUAL;
  applyRelay1(on);
  Serial.print("Relay1: "); Serial.println(on ? "ON" : "OFF");
}

void setRelay2(bool on) {
  currentMode = MODE_MANUAL;
  applyRelay2(on);
  Serial.print("Relay2: "); Serial.println(on ? "ON" : "OFF");
}

void setBrightness(int percent) {
  currentMode = MODE_MANUAL;
  int pwm = map(constrain(percent, 0, 100), 0, 100, 0, 255);
  smoothPWM(pwm);
  Serial.print("Brightness: "); Serial.println(percent);
}

void setAllOff() {
  smoothPWM(0);
  applyRelay1(false);
  applyRelay2(false);
  Serial.println("All lights OFF");
}

// ─── Getters ─────────────────────────────────────
LightMode getCurrentMode()     { return currentMode;    }
bool      getRelay1State()     { return relay1State;    }
bool      getRelay2State()     { return relay2State;    }
int       getBrightnessPercent(){ return map(currentPWM, 0, 255, 0, 100); }
float     getLux()             { return currentLux;     }
bool      isPersonDetected()   { return personDetected; }