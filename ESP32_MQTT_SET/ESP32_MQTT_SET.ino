#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

// WiFi credentials
const char* ssid = "Pepper_C1-MUX-88B704";
const char* password = "";

// MQTT broker settings
const char* mqtt_server = "192.168.100.2";  // Change to your broker
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "smartenergytable";
const char* mqtt_topic = "esp/test";

WiFiClient espClient;
PubSubClient client(espClient);

const int LED_PINS[] = {2, 25, 26};  // add more 
const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const unsigned long timerDuration = 250;

bool ledOn[NUM_LEDS] = {false};
unsigned long timer_Start[NUM_LEDS] = {0};


void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);

   // Split message into command and antenna number
  int separatorIndex = message.indexOf(':');  // Find the colon position
  String command = message.substring(0, separatorIndex);  // Extract command
  int antenna = message.substring(separatorIndex + 1).toInt();  // Extract antenna number

    if (antenna >= 1 && antenna <= NUM_LEDS) {
            int ledIndex = antenna - 1; // starts from 0
            digitalWrite(LED_PINS[ledIndex], HIGH);
            ledOn[ledIndex] = true;
            timer_Start[ledIndex] = millis();
        }
    for (int i = 0; i < NUM_LEDS; i++) {
        if (ledOn[i] && (millis() - timer_Start[i] >= timerDuration)) {
            digitalWrite(LED_PINS[i], LOW);
            ledOn[i] = false;
        }
    }
   
   //Serial.println("yes");
}

void setup_wifi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {  // Authenticate here
            Serial.println("Connected to MQTT broker");
            client.subscribe(mqtt_topic);
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < NUM_LEDS; i++) {
        pinMode(LED_PINS[i], OUTPUT);
    }
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
}
