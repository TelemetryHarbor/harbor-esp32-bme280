#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h> // For NTP time

// WiFi credentials
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// Telemetry Harbor API info
const String apiUrl = "https://telemetryharbor.com/api/v1/ingest/ingest/Harbor_ID";
const String apiKey = "API_KEY";
const String shipId = "Living Room";

// BME280 setup
Adafruit_BME280 bme; // I2C
#define SEALEVELPRESSURE_HPA (1013.25)

// LED setup
const int ledPin = 2; // Use built-in LED on most ESP32 boards (usually GPIO 2)

// NTP setup
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0; // Adjust this for your timezone offset
const int daylightOffset_sec = 0;

// Time management
unsigned long previousMillis = 0;
const unsigned long intervalMillis = 60000; // 1-second interval

// Function to flash the LED
void flashLED(int times, int delayMs) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(delayMs);
    digitalWrite(ledPin, LOW);
    delay(delayMs);
  }
}

// Function to connect to Wi-Fi
void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi!");
  flashLED(3, 200); // Flash LED 3 times quickly
}

// Function to initialize time using NTP
void initTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time from NTP server.");
    while (1); // Stay here if time sync fails
  }
  Serial.println("Time synchronized successfully.");
}

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT); // Set LED pin as output

  // Initialize BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Connect to Wi-Fi and sync time
  connectWiFi();
  initTime();
}

void loop() {
  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi(); // Reconnect if disconnected
  }

  // Measure elapsed time to push data every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= intervalMillis) {
    previousMillis = currentMillis;


    const float temperatureOffset = 0; // Adjust temperature by -3°C
    const float humidityOffset = 0;    // Adjust humidity by +7.2%
    // Read temperature and humidity
    float temperature = bme.readTemperature() + temperatureOffset;
    float humidity = bme.readHumidity() + humidityOffset;

    // Get current time in ISO8601 format
    time_t now = time(NULL);
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo); // Convert to UTC time structure

    char timeBuffer[30];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo); // Format time

    // Create JSON payload for temperature
    String tempJsonPayload = "{";
    tempJsonPayload += "\"time\": \"" + String(timeBuffer) + "\",";
    tempJsonPayload += "\"ship_id\": \"" + shipId + "\",";
    tempJsonPayload += "\"cargo_id\": \"Temperature\",";
    tempJsonPayload += "\"value\": " + String(temperature, 2);
    tempJsonPayload += "}";

    // Create JSON payload for humidity
    String humidityJsonPayload = "{";
    humidityJsonPayload += "\"time\": \"" + String(timeBuffer) + "\",";
    humidityJsonPayload += "\"ship_id\": \"" + shipId + "\",";
    humidityJsonPayload += "\"cargo_id\": \"Humidity\",";
    humidityJsonPayload += "\"value\": " + String(humidity, 2);
    humidityJsonPayload += "}";

    // Send temperature data to API
    HTTPClient http;
    http.begin(apiUrl);
    http.addHeader("X-API-Key", apiKey);
    http.addHeader("Content-Type", "application/json");

    int tempResponseCode = http.POST(tempJsonPayload);
    if (tempResponseCode > 0) {
      Serial.println("Temperature data sent successfully! Response:");
      Serial.println(http.getString());
      flashLED(1, 300); // Flash LED for successful API request
    } else {
      Serial.print("Error sending temperature data. HTTP Response code: ");
      Serial.println(tempResponseCode);
    }
    http.end();

    // Send humidity data to API
    http.begin(apiUrl); // Reinitialize HTTPClient for humidity request
    http.addHeader("X-API-Key", apiKey);
    http.addHeader("Content-Type", "application/json");

    int humidityResponseCode = http.POST(humidityJsonPayload);
    if (humidityResponseCode > 0) {
      Serial.println("Humidity data sent successfully! Response:");
      Serial.println(http.getString());
      flashLED(1, 300); // Flash LED for successful API request
    } else {
      Serial.print("Error sending humidity data. HTTP Response code: ");
      Serial.println(humidityResponseCode);
    }
    http.end();
  }
}
