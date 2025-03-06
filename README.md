# ESP32 BME280 Telemetry Monitor

Real-time environmental monitoring made easy with ESP32, BME280, and Telemetry Harbor.

## Overview

This project turns an ESP32 microcontroller and BME280 sensor into a powerful cloud-connected environmental monitoring system. Track temperature and humidity in real-time from anywhere in the world, with data beautifully visualized through Telemetry Harbor and Grafana dashboards.

## Features

- **Real-time monitoring**: Captures temperature and humidity data at customizable intervals
- **Cloud connectivity**: Seamless data transmission to Telemetry Harbor
- **Beautiful visualizations**: Ready-to-use Grafana dashboards
- **Expandable**: Add more sensors or extend functionality
- **Configurable**: Easily adjust sampling rates, reporting intervals, and sensor calibration

## Hardware Requirements

| Component | Description | Approximate Cost |
|-----------|-------------|------------------|
| ESP32 | Development board (NodeMCU ESP32, WROOM, etc.) | $3.82 (AliExpress) / $9.90 (Amazon US) |
| BME280 | Temperature, humidity, and pressure sensor module | $4.20 (AliExpress) / $8.99 (Amazon US) |
| Jumper wires | For connecting components | $0.42 (AliExpress) / $5.49 (Amazon US) |
| Micro USB cable | For power & programming | $3.00 (AliExpress) / $5.00 (Amazon US) |
| *Optional* | 3.3V power supply for standalone deployment | |

## Software Requirements

- Arduino IDE (1.8.13 or newer) or PlatformIO
- ESP32 board support package
- Required libraries:
  - Adafruit BME280 Library
  - Adafruit Unified Sensor
  - WiFi Library (built into ESP32 core)
  - HTTPClient Library (built into ESP32 core)
- [Telemetry Harbor](https://telemetryharbor.com) account (free tier available)

## Wiring Diagram

Connect the BME280 sensor to the ESP32 using I2C:

| BME280 Pin | ESP32 Pin | Function |
|------------|-----------|----------|
| VCC | 3.3V | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | Data |
| SCL | GPIO 22 | Clock |

## Setup Instructions

### 1. Hardware Assembly

1. Connect the BME280 sensor to the ESP32 according to the wiring diagram
2. Connect the ESP32 to your computer via USB

### 2. Software Setup

1. Install the Arduino IDE
2. Add ESP32 board support by adding this URL in Preferences > Additional Boards Manager URLs:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Install the required libraries via the Library Manager:
   - Adafruit BME280 Library
   - Adafruit Unified Sensor
4. Clone this repository or download the code

### 3. Telemetry Harbor Configuration

1. Create a [Telemetry Harbor](https://telemetryharbor.com) account
2. Create a new Harbor called "ESP32_Environmental_Monitor" (or your preferred name)
3. Select "General" harbor type and "Free" specification
4. Copy your API Key and Harbor ID from the Harbor details page

### 4. Configure and Upload the Code

1. Open the `ESP32_BME280_Telemetry.ino` file
2. Update the following variables:
   - WiFi SSID and password
   - Telemetry Harbor API Endpoint URL and API Key
   - Adjust the `shipId` and sensor reading interval if desired
3. Select your ESP32 board model from Tools > Board
4. Select the correct COM port from Tools > Port
5. Upload the code

### 5. Visualize Your Data

1. Access your Telemetry Harbor account
2. Navigate to the Harbor details page and copy your Grafana password
3. Access the Grafana dashboard using the provided endpoint
4. Login with your Telemetry Harbor email and the Grafana password
5. Navigate to Dashboards and select the Comprehensive Telemetry Dashboard
6. Use filters to view your specific sensor data

## Customization

### Sensor Calibration

You can adjust the temperature and humidity offsets in the code:

```cpp
const float temperatureOffset = 0; // Adjust as needed
const float humidityOffset = 0;    // Adjust as needed
```

### Data Collection Interval

Modify the `intervalMillis` variable to change how frequently data is collected and sent:

```cpp
const unsigned long intervalMillis = 60000; // 60000ms = 1 minute
```

## Troubleshooting

### Common Issues

- **BME280 not found**: Double-check wiring or try alternate I2C address (0x76 or 0x77)
- **WiFi connection failure**: Verify credentials and ESP32 WiFi capability
- **Data not appearing in Telemetry Harbor**: Verify API key and URL are correct

### Debug Mode

For detailed debugging, monitor the serial output at 115200 baud.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- [Adafruit](https://adafruit.com) for their excellent BME280 library
- [Espressif](https://espressif.com) for the ESP32 platform
- [Telemetry Harbor](https://telemetryharbor.com) for the IoT data platform
