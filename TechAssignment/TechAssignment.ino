#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4 
#define DHTTYPE DHT11 

const char* ssid = "drichz wifi";
const char* password = "ald112207";
const char* mqttServer = "broker.mqtt.cool";
const int mqttPort = 1883;
const char* mqttTopic = "temperatureTopic";

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  dht.begin();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqttServer, mqttPort);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    while (!client.connected()) {
      Serial.println("Reconnecting to MQTT...");
      if (client.connect("ESP32Client")) {
        Serial.println("Reconnected to MQTT");
      } else {
        Serial.print("Failed with state ");
        Serial.print(client.state());
        delay(2000);
      }
    }
  }
  
  client.loop();

  float temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String temperatureString = String(temperature);
  client.publish("temperatureTopic", temperatureString.c_str());

  delay(15000); 
}
