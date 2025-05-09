#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Setup WiFi and MQTT parameters
const char *ssid = "Wokwi-GUEST"; // Replace with your network SSID
const char *password = "";        // Replace with your network password

const char *mqtt_server = "0.tcp.ap.ngrok.io"; // Ngrok tunnel address
const int mqtt_port = 18085;                   // Ngrok tunnel port
const char *mqtt_user = "58caa097-8955-6304-245";
const char *mqtt_password = "a00d1d3";
const char *mqtt_topic = "devices/telemetry";
const char *mqtt_client_id = "mqtt_3affeab3-49c";

WiFiClient espClient;
PubSubClient client(espClient);

// DHT22 sensor setup
const int pinDHT22 = 15;
#define DHTTYPE DHT22
DHT dht(pinDHT22, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("DHT22 Test!");

    // Initialize DHT sensor
    dht.begin();

    // Check wifi connection
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Check MQTT connection
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    // Read DHT22 sensor
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if any reads failed
    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read from DHT sensor!");
        delay(2000);
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Convert data to JSON and publish to MQTT
    StaticJsonDocument<200> doc; // Increased size to ensure enough space
    doc["humidity"] = humidity;
    doc["temperature"] = temperature;

    char msg[200]; // Increased buffer size
    serializeJson(doc, msg);

    if (client.publish(mqtt_topic, msg))
    {
        Serial.println("Message published successfully");
    }
    else
    {
        Serial.println("Failed to publish message");
    }

    delay(5000); // Changed to 5 seconds to match fake-data.go
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        if (client.connect(mqtt_client_id, mqtt_user, mqtt_password))
        {
            Serial.println("connected");
            // Subscribe to topics if needed
            // client.subscribe("some/topic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}