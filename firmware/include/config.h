#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// MQTT Configuration
#define MQTT_SERVER "192.168.1.100"  // Replace with your MQTT broker IP
#define MQTT_PORT 1883
#define MQTT_USER "your_mqtt_user"    // Replace with your MQTT username
#define MQTT_PASSWORD "your_mqtt_pass" // Replace with your MQTT password
#define DEVICE_ID "esp32_sensor_01"

// GPIO Configuration
#define DHT_PIN 4
#define RELAY_PIN 23
#define DHT_TYPE DHT22

// Timing Configuration (milliseconds)
#define SENSOR_INTERVAL 10000        // 10 seconds
#define WIFI_RETRY_INTERVAL 30000    // 30 seconds
#define MQTT_RETRY_INTERVAL 5000     // 5 seconds

// MQTT Topics
#define TOPIC_SENSOR_DATA "sensors/esp32_sensor_01/data"
#define TOPIC_RELAY_COMMAND "sensors/esp32_sensor_01/relay/cmd"
#define TOPIC_RELAY_STATUS "sensors/esp32_sensor_01/relay/status"
#define TOPIC_STATUS "sensors/esp32_sensor_01/status"

#endif // CONFIG_H