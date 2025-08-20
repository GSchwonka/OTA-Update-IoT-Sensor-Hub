# ESP32 Smart Home Controller (Wi-Fi + MQTT)

## 📌 Project Overview
This project implements a **Smart Home Controller** using an **ESP32** microcontroller.  
The device connects to Wi-Fi and communicates with an **MQTT broker** to control a **relay (GPIO 23)** for switching a lamp on/off, and to publish temperature/humidity readings from a **DHT22 sensor (GPIO 4)** every 10 seconds.  

It includes automatic reconnection for Wi-Fi and MQTT, ensuring reliable operation in real-world scenarios.

---

## ✨ Features
- ✅ Relay control (GPIO 23) via MQTT commands (`home/relay/set`).  
- ✅ DHT22 sensor readings (temperature & humidity) published every 10 seconds to `home/sensor/dht22`.  
- ✅ MQTT last will & testament (LWT) for device availability.  
- ✅ Automatic reconnection on Wi-Fi or MQTT disconnection.  
- ✅ Configurable Wi-Fi and MQTT credentials.  

---

## 🛠️ Hardware
- ESP32 DevKit (e.g., WROOM-32)  
- DHT22 sensor (GPIO 4)  
- Relay module (GPIO 23)  
- Micro USB cable  
- Breadboard + jumper wires  

---

## 💻 Software & Libraries
- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE  
- [PubSubClient](https://pubsubclient.knolleary.net/) (MQTT)  
- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)  
- WiFi library (built-in with ESP32 Arduino Core)  

---

## 📂 Repo Structure
```
/firmware
  ├── src/
  │   └── main.cpp
  ├── include/
  ├── platformio.ini
/docs
/assets
README.md
```

---

## ⚡ Quick Start
1. **Clone the repository**  
   ```bash
   git clone https://github.com/<your-username>/esp32-smart-home-controller.git
   cd esp32-smart-home-controller/firmware
   ```

2. **Configure Wi-Fi & MQTT**  
   Edit `include/config.h` (create if missing):
   ```cpp
   #define WIFI_SSID     "your-ssid"
   #define WIFI_PASS     "your-password"
   #define MQTT_SERVER   "192.168.1.100"
   #define MQTT_PORT     1883
   #define MQTT_USER     "mqtt-user"
   #define MQTT_PASS     "mqtt-pass"
   ```

3. **Build & Flash**  
   ```bash
   pio run -t upload
   ```

4. **Monitor Serial Logs**  
   ```bash
   pio device monitor -b 115200
   ```

---

## 📡 MQTT Topics
- **Publish (sensor data):**
  - `home/sensor/dht22/temperature`
  - `home/sensor/dht22/humidity`

- **Subscribe (relay control):**
  - `home/relay/set` → payload: `"ON"` or `"OFF"`

- **Device status:**
  - `home/device/status` → `"online"` / `"offline"`

---

## 🧪 Testing
- Use [MQTT Explorer](http://mqtt-explorer.com/) or `mosquitto_sub` to subscribe to sensor topics.  
- Publish test commands to toggle relay:  
  ```bash
  mosquitto_pub -h 192.168.1.100 -t "home/relay/set" -m "ON"
  mosquitto_pub -h 192.168.1.100 -t "home/relay/set" -m "OFF"
  ```

---

## 📖 Notes on Design Choices
- **MQTT LWT** ensures smart home platforms know if device is disconnected.  
- **Automatic reconnection** makes the system resilient to Wi-Fi dropouts.  
- **Config header file** makes it easier to change credentials without editing main code.  
