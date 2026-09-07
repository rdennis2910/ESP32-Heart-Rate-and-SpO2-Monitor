#include <Wire.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ESP32Dashboard.h"
#include <WiFi.h>
#include <ESPmDNS.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D

const int validHRMin = 40;
const int validHRMax = 200;
const int validSPO2Min = 70;
const int validSPO2Max = 100;
const int sampleCount = 100;

const char* WIFI_SSID = "Your-WIFI-Name";
const char* WIFI_PASSWORD = "Your-WIFI-Password";
const char* hostName = "esp32";

byte readLED = 13;
byte pulseLED = 11;
byte validReadingLED = 10;
byte invalidHRLED = 9;
byte invalidSPO2LED = 4;
byte bothInvalidLED = 19;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)

uint16_t irBuffer[sampleCount];
uint16_t redBuffer[sampleCount];
#else
uint32_t irBuffer[sampleCount];
uint32_t redBuffer[sampleCount];
#endif

int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
int lastHeartRate;
int lastSPO2;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX30105 particleSensor;
ESP32Dashboard dashboard;

void setup()
{
  Serial.begin(115200);
  WiFi.setHostname(hostName);
  Wire.begin(21, 22);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Pulse:");
  display.println("");
  display.println("SPO2:");
  display.display();
  pinMode(pulseLED, OUTPUT);
  pinMode(readLED, OUTPUT);

  pinMode(invalidSPO2LED, OUTPUT);
  pinMode(invalidHRLED, OUTPUT);
  pinMode(validReadingLED, OUTPUT);
  pinMode(bothInvalidLED, OUTPUT);

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Check if Max30105 sensor is working
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }

  byte ledBrightness = 60;
  byte sampleAverage = 4;
  byte ledMode = 2;
  byte sampleRate = 100;
  int pulseWidth = 411;
  int adcRange = 4096;

  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);

  if (!dashboard.begin(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("WiFi connection failed!");
    while (true) delay(1000);
  }

  if (MDNS.begin(hostName)) { // Starts website with custom name
    Serial.print("mDNS responder started. Access via: http://");
    Serial.print(hostName);
    Serial.println(".local");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }
  
  dashboard.setTitle("ESP32 Heart Rate and SPO2 Sensor");
  dashboard.setUpdateInterval(500);
  
  dashboard.addCustomCard("Heart Rate", "Live Heart Rate", []() {
  return String(constrain(lastHeartRate, validHRMin, validHRMax)) + " BPM";
}, []() {
  return validHeartRate ? "Valid Reading" : "Invalid Reading";
}, "red", "❤️");

  dashboard.addCustomCard("SpO2%", "Live SpO2%", []() {
  return String(constrain(lastSPO2, validSPO2Min, validSPO2Max)) + "%";
}, []() {
  return validSPO2 ? "Valid" : "Invalid";
}, "blue", "🫁");

  dashboard.addChartCard("Heart Rate Chart in BPM", "Live Pulse Reading", []() {
    return constrain(lastHeartRate, validHRMin, validHRMax);
  }, "red", 40);

  dashboard.addChartCard("SpO2% Chart", "Live SpO2% Reading", []() {
    return constrain(lastSPO2, validSPO2Min, validSPO2Max);
  }, "blue", 40);
}

void loop()
{

  for (byte i = 0 ; i < sampleCount ; i++)
  {
    while (particleSensor.available() == false)
      particleSensor.check();

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, sampleCount, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate); // Calculates initial heart rate and SPO2% from samples

  while (1)
  {
    display.clearDisplay();

    for (byte i = 25; i < 100; i++) // Shifts the most recent 75 samples to allow room for 25 more samples
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    for (byte i = 75; i < 100; i++) // Sensor collects 25 new samples
    {
      while (particleSensor.available() == false)
        particleSensor.check();

      digitalWrite(readLED, !digitalRead(readLED));

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
    }
    maxim_heart_rate_and_oxygen_saturation(irBuffer, sampleCount, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate); // Recalculates heart rate and SPO2% from the new samples

    if (validHeartRate && validSPO2) { //Turns on or off the corresponding LEDS based on the validity of the new measurements
      digitalWrite(validReadingLED, HIGH);
      digitalWrite(invalidHRLED, LOW);
      digitalWrite(invalidSPO2LED, LOW);
      digitalWrite(bothInvalidLED, LOW);
    }
    else if (!validSPO2 && !validHeartRate) {
      digitalWrite(invalidSPO2LED, LOW);
      digitalWrite(invalidHRLED, LOW);
      digitalWrite(validReadingLED, LOW);
      digitalWrite(bothInvalidLED, HIGH);
    }
    else if (!validHeartRate) {
      digitalWrite(invalidHRLED, HIGH);
      digitalWrite(validReadingLED, LOW);
      digitalWrite(invalidSPO2LED, LOW);
      digitalWrite(bothInvalidLED, LOW);
    }
    else if (!validSPO2) {
      digitalWrite(invalidSPO2LED, HIGH);
      digitalWrite(invalidHRLED, LOW);
      digitalWrite(validReadingLED, LOW);
      digitalWrite(bothInvalidLED, LOW);
    }

    if (validHeartRate && heartRate >= validHRMin && heartRate <= validHRMax ){ // Only updates the display when the measurements are within the expected range and valid
      lastHeartRate = heartRate;
    }

    if (validSPO2 && spo2 >= validSPO2Min && spo2 <= validSPO2Max){
      lastSPO2 = spo2;
    }

    display.setCursor(0,0);
    display.print("Pulse:");
    display.print(lastHeartRate);
    display.println("");
    display.println("");
    display.print("SpO2:");
    display.print(lastSPO2);
    display.print("%");
    display.display();

    dashboard.loop();
    static unsigned long last = 0;
    if (millis() - last > 5000) {
      dashboard.printSystemStatus();
      last = millis();
    }
  }
}