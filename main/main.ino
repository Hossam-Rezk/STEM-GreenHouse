#include <WiFi.h>
#include <DHT.h>
#include <WebServer.h>

// DHT22 Setup
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Soil Moisture Sensor
#define MOISTURE_PIN 34

// PIR Sensor for Motion Detection
#define PIR_PIN 32

// Relay for Fan Control
#define RELAY_PIN 33

// Buzzer
#define BUZZER_PIN 25

// Wi-Fi Credentials
const char* ssid = "STEM19EE";
const char* password = "1919016111@H";

// Threshold Values
const float tempThreshold = 30.0;    // Temperature in °C
const int soilMoistureThreshold = 30; // Soil moisture percentage
const bool motionThreshold = true;  // Motion detected

// Web Server
WebServer server(80);

// Initialize Sensors and Actuators
void initSensors() {
  dht.begin();
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
}

// Fetch Sensor Data
String getSensorDataJson() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(MOISTURE_PIN);
  bool motionDetected = digitalRead(PIR_PIN);

  // Convert soil moisture to percentage
  int soilMoisturePercent = map(soilMoisture, 0, 4095, 0, 100);

  // Check thresholds
  bool thresholdExceeded = false;

  // Control fan based on temperature
  if (temperature > tempThreshold) {
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  // Check soil moisture threshold
  if (soilMoisturePercent < soilMoistureThreshold) {
    thresholdExceeded = true;
  }

  // Check motion detection
  if (motionDetected == motionThreshold) {
    thresholdExceeded = true;
  }

  // Control buzzer based on any threshold exceeded
  digitalWrite(BUZZER_PIN, thresholdExceeded ? HIGH : LOW);

  // Print readings to Serial Monitor
  Serial.println("==== Sensor Readings ====");
  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisturePercent);
  Serial.println(" %");

  Serial.print("Motion Detected: ");
  Serial.println(motionDetected ? "Yes" : "No");

  Serial.print("Threshold Exceeded: ");
  Serial.println(thresholdExceeded ? "Yes" : "No");
  Serial.println("========================");

  // Generate JSON string for web server
  String json = "{";
  json += "\"temperature\": " + String(temperature, 1) + ",";
  json += "\"humidity\": " + String(humidity, 1) + ",";
  json += "\"soilMoisture\": " + String(soilMoisturePercent) + ",";
  json += "\"motionDetected\": " + String(motionDetected ? "true" : "false") + ",";
  json += "\"thresholdExceeded\": " + String(thresholdExceeded ? "true" : "false");
  json += "}";

  return json;
}

// Serve Sensor Data
void handleSensorData() {
  String data = getSensorDataJson();
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", data);
}

// Wi-Fi Connection
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Start Web Server
void startServer() {
  server.on("/data", handleSensorData);
  server.begin();
  Serial.println("Web Server started");
}

void setup() {
  Serial.begin(115200);
  
  connectToWiFi();
  initSensors();
  startServer();
}

void loop() {
  server.handleClient();
}
