#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SimpleDHT.h>
#include <ArduinoJson.h>


// Setup WiFi and MQTT parameters
const char* ssid = "your_SSID"; // Replace with your network SSID
const char* password = "your_PASSWORD"; // Replace with your network password

const char* mqtt_server = "192.168.1.13:1883"; // ipconfig getifaddr en0 
const char* mqtt_user = "02d0422c-0c91-454a-e89"; 
const char* mqtt_password = "7febfd7";
const char* mqtt_topic = "device/attributes";

WiFiClient espClient;
PubSubClient client(espClient);

// DHT11 sensor setup
const int pinDHT11 = 5;
SimpleDHT11 dht11(pinDHT11);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("DHT11 Test!");

  // Check wifi connection
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Check MQTT connection
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read DHT11 sensor
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
    return;
  }
  
  Serial.print("Temperature: ");
  Serial.print((int)temperature); 
  Serial.println(" *C");
  
  Serial.print("Humidity: ");
  Serial.print((int)humidity); 
  Serial.println(" %");

  // Convert data to JSON and publish to MQTT
  StaticJsonDocument<100> doc;
  doc["temperature"] = (int)temperature;
  doc["humidity"] = (int)humidity;
  char msg[100];
  serializeJson(doc, msg);
  client.publish(mqtt_topic, msg);

  delay(2000);  // Delay 2 seconds between readings
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
