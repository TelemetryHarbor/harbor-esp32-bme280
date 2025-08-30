#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include "HarborClient.h"
#include <time.h>

// WiFi credentials
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

// Telemetry Harbor API info
const char* harborEndpoint = "API_ENDPOINT";
const char* harborApiKey   = "API_KEY";
const char* shipId         = "Living Room";

HarborClient harbor(harborEndpoint, harborApiKey);

// BME280 setup
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1013.25)

// Offsets
const float TEMP_OFFSET = 0.0;     // e.g., -3.0
const float HUMIDITY_OFFSET = 0.0; // e.g., +7.2
const float PRESSURE_OFFSET = 0.0; // hPa, e.g., -2.5
const float ALTITUDE_OFFSET = 0.0; // meters, e.g., +5.0

// LED (GPIO2) off
const int ledPin = 2;

// NTP setup
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// Sleep timing
const uint64_t uS_TO_S_FACTOR = 1000000ULL;
const int SLEEP_INTERVAL_SEC = 5 * 60; // 5 minutes

// Get current UTC time as ISO8601
String getISOTime() {
  time_t now = time(NULL);
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  char buf[30];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}

// Connect WiFi with minimal retries
bool connectWiFi() {
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 15) {
    delay(200);
    retries++;
  }
  return WiFi.status() == WL_CONNECTED;
}

void setup() {
  // Reduce CPU frequency to save power during wake
  setCpuFrequencyMhz(80);

  // Disable user LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Minimal serial
  Serial.begin(115200);
  delay(100);

  // Init BME280 in forced mode
  if (!bme.begin(0x76)) {
    esp_deep_sleep(SLEEP_INTERVAL_SEC * uS_TO_S_FACTOR); // try again later
  }
  bme.setSampling(Adafruit_BME280::MODE_FORCED);

  // Connect Wi-Fi
  if (!connectWiFi()) {
    esp_deep_sleep(SLEEP_INTERVAL_SEC * uS_TO_S_FACTOR);
  }

  // Sync time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    esp_deep_sleep(SLEEP_INTERVAL_SEC * uS_TO_S_FACTOR);
  }

  // Read sensors (forced mode → sleeps after reading)
  bme.takeForcedMeasurement();
  float temperature = bme.readTemperature() + TEMP_OFFSET;
  float humidity    = bme.readHumidity() + HUMIDITY_OFFSET;
  float pressure    = (bme.readPressure() / 100.0F) + PRESSURE_OFFSET;
  float altitude    = bme.readAltitude(SEALEVELPRESSURE_HPA) + ALTITUDE_OFFSET;

  String isoTime = getISOTime();

  // Prepare batch
  GeneralReading readings[4];
  readings[0] = {shipId, "Temperature", temperature, isoTime};
  readings[1] = {shipId, "Humidity", humidity, isoTime};
  readings[2] = {shipId, "Pressure", pressure, isoTime};
  readings[3] = {shipId, "Altitude", altitude, isoTime};

  // Send batch
  harbor.sendBatch(readings, 4);

  // Disconnect Wi-Fi before sleep
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  // Deep sleep for 5 minutes
  esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_SEC * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void loop() {
  // never used, everything runs in setup + deep sleep
}
