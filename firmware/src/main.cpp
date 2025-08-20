#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Configuration constants
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* MQTT_SERVER = "192.168.1.100";  // Replace with your MQTT broker IP
const int MQTT_PORT = 1883;
const char* MQTT_USER = "your_mqtt_user";    // Replace with your MQTT username
const char* MQTT_PASSWORD = "your_mqtt_pass"; // Replace with your MQTT password
const char* DEVICE_ID = "esp32_sensor_01";

// GPIO pins
#define DHT_PIN 4
#define RELAY_PIN 23
#define DHT_TYPE DHT22

// MQTT topics
const char* TOPIC_SENSOR_DATA = "sensors/esp32_sensor_01/data";
const char* TOPIC_RELAY_COMMAND = "sensors/esp32_sensor_01/relay/cmd";
const char* TOPIC_RELAY_STATUS = "sensors/esp32_sensor_01/relay/status";
const char* TOPIC_STATUS = "sensors/esp32_sensor_01/status";

// Timing constants
const unsigned long SENSOR_INTERVAL = 10000; // 10 seconds
const unsigned long WIFI_RETRY_INTERVAL = 30000; // 30 seconds
const unsigned long MQTT_RETRY_INTERVAL = 5000;  // 5 seconds

// Global objects
WiFiClient espClient;
PubSubClient mqttClient(espClient);
DHT dht(DHT_PIN, DHT_TYPE);

// Global variables
unsigned long lastSensorReading = 0;
unsigned long lastWifiRetry = 0;
unsigned long lastMqttRetry = 0;
bool relayState = false;
bool wifiConnected = false;
bool mqttConnected = false;

// Function prototypes
void connectToWiFi();
void connectToMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleRelayCommand(String command);
void readAndPublishSensorData();
void setRelay(bool state);
void publishStatus(const char* status);

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 IoT Sensor Hub Starting ===");
  
  // Initialize GPIO pins
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Start with relay OFF
  
  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT22 sensor initialized");
  
  // Initialize MQTT client
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  
  // Start WiFi connection
  connectToWiFi();
  
  Serial.println("Setup complete!");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Check WiFi connection
  if (!wifiConnected && (currentTime - lastWifiRetry >= WIFI_RETRY_INTERVAL)) {
    connectToWiFi();
    lastWifiRetry = currentTime;
  }
  
  // Check MQTT connection
  if (wifiConnected && !mqttConnected && (currentTime - lastMqttRetry >= MQTT_RETRY_INTERVAL)) {
    connectToMQTT();
    lastMqttRetry = currentTime;
  }
  
  // Handle MQTT loop
  if (mqttConnected) {
    mqttClient.loop();
  }
  
  // Read and publish sensor data
  if (currentTime - lastSensorReading >= SENSOR_INTERVAL) {
    readAndPublishSensorData();
    lastSensorReading = currentTime;
  }
  
  // Small delay to prevent watchdog reset
  delay(100);
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    wifiConnected = false;
    Serial.println();
    Serial.println("WiFi connection failed!");
  }
}

void connectToMQTT() {
  Serial.print("Connecting to MQTT broker: ");
  Serial.println(MQTT_SERVER);
  
  if (mqttClient.connect(DEVICE_ID, MQTT_USER, MQTT_PASSWORD)) {
    mqttConnected = true;
    Serial.println("MQTT connected successfully!");
    
    // Subscribe to relay command topic
    mqttClient.subscribe(TOPIC_RELAY_COMMAND);
    Serial.print("Subscribed to: ");
    Serial.println(TOPIC_RELAY_COMMAND);
    
    // Publish online status
    publishStatus("online");
    
  } else {
    mqttConnected = false;
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  // Convert payload to string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message: ");
  Serial.println(message);
  
  // Handle relay commands
  if (String(topic) == TOPIC_RELAY_COMMAND) {
    handleRelayCommand(message);
  }
}

void handleRelayCommand(String command) {
  command.toLowerCase();
  command.trim();
  
  if (command == "on" || command == "1" || command == "true") {
    setRelay(true);
  } else if (command == "off" || command == "0" || command == "false") {
    setRelay(false);
  } else if (command == "toggle") {
    setRelay(!relayState);
  } else {
    Serial.print("Unknown relay command: ");
    Serial.println(command);
  }
}

void setRelay(bool state) {
  relayState = state;
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);
  
  Serial.print("Relay turned ");
  Serial.println(state ? "ON" : "OFF");
  
  // Publish relay status
  if (mqttConnected) {
    mqttClient.publish(TOPIC_RELAY_STATUS, state ? "ON" : "OFF", true);
  }
}

void readAndPublishSensorData() {
  // Read DHT22 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Serial.println("=== Sensor Reading ===");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println("°C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  
  // Create JSON payload
  JsonDocument doc;
  doc["device_id"] = DEVICE_ID;
  doc["timestamp"] = millis();
  doc["temperature"] = round(temperature * 100) / 100.0; // Round to 2 decimal places
  doc["humidity"] = round(humidity * 100) / 100.0;
  doc["relay_state"] = relayState ? "ON" : "OFF";
  doc["wifi_rssi"] = WiFi.RSSI();
  
  // Serialize JSON
  String jsonString;
  serializeJson(doc, jsonString);
  
  Serial.print("JSON payload: ");
  Serial.println(jsonString);
  
  // Publish to MQTT if connected
  if (mqttConnected) {
    if (mqttClient.publish(TOPIC_SENSOR_DATA, jsonString.c_str())) {
      Serial.println("Sensor data published successfully");
    } else {
      Serial.println("Failed to publish sensor data");
      mqttConnected = false; // Trigger reconnection
    }
  } else {
    Serial.println("MQTT not connected - data not published");
  }
}

void publishStatus(const char* status) {
  if (mqttConnected) {
    JsonDocument doc;
    doc["device_id"] = DEVICE_ID;
    doc["status"] = status;
    doc["timestamp"] = millis();
    doc["ip_address"] = WiFi.localIP().toString();
    doc["wifi_rssi"] = WiFi.RSSI();
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    mqttClient.publish(TOPIC_STATUS, jsonString.c_str(), true);
    Serial.print("Status published: ");
    Serial.println(status);
  }
}