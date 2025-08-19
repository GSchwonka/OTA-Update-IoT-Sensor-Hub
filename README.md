# OTA-Update IoT Sensor Hub

## Project Title
OTA-Update IoT Sensor Hub

## One-line Description
ESP32-based multi-sensor node with secure OTA updates, WebSocket/MQTT telemetry, and a lightweight dashboard.

## Features
- Reads temperature, humidity and IMU sensors.
- Local async web dashboard (WebSocket) for live graphs.
- MQTT telemetry to a Raspberry Pi broker.
- Secure OTA (HTTP or MQTT-based) with rollback on failure.
- Basic power management and watchdogs.

## Hardware
- ESP32 dev board (e.g., ESP32-WROOM-32)
- BME280 (temp/humidity/pressure) or DHT22
- MPU6050 IMU (optional)
- Micro USB cable
- Optional: SD card module for local logging

## Software / Libraries
- PlatformIO or Arduino IDE
- AsyncTCP, ESPAsyncWebServer (or native alternatives)
- PubSubClient or AsyncMQTT
- ArduinoOTA (or custom HTTP OTA implementation)

## Repo Structure
```
/firmware
  /src
  /include
/server
  /mqtt-broker-setup
/docs
/assets
README.md
```

## Quick Start (flash & run)
1. Clone repo
2. Configure `platformio.ini` or `boards.txt` with your board
3. Edit `config/defaults.h` for Wi-Fi and MQTT settings
4. `pio run -t upload` (or flash from Arduino IDE)
5. Access device dashboard at `http://<device-ip>`

## OTA Procedure
- Upload new firmware binary to `server/firmware/` and trigger OTA via the Pi tool or MQTT topic `device/ota/<device-id>`.
- Device validates checksum and applies update, if fails it rolls back to previous firmware.

## Diagnostics & Troubleshooting
- Serial logs: 115200 bps
- Watchdog status exposed at `GET /status`
- Common issues: incorrect MQTT credentials, MQTT broker unreachable, OTA checksum mismatch

## Demo
Add `assets/demo.gif` showing live dashboard and OTA sequence.

## Notes on Design Choices
- Why WebSocket? Low-latency live graphs without polling.
- Why rollback? Field devices must not brick during updates.

## License
MIT
