#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

// LED definintions
#define NUM_LEDS 120
#define Data_Pin 25
#define chipset WS2812B
#define BRIGHTNESS 50
#define LEDSECTIONS 3

// LED setting sections
CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet leds1(leds(0, 7));
CRGBSet leds2(leds(8, 15));
CRGBSet leds3(leds(16, 23));

struct CRGB *ledarray[] = {leds1, leds2, leds3};
uint8_t sizearray[] = {8, 8, 8}; 

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

//const int LED_PINS[] = {2, 25, 26};  // add more 
//const int NUM_LEDS = sizeof(LED_PINS) / sizeof(LED_PINS[0]);
const unsigned long timerDuration = 250;

bool ledOn[NUM_LEDS] = {false};
unsigned long timer_Start[NUM_LEDS] = {0};

// LED structure atributes
struct WipeEffect {
  uint8_t index = 0;
  bool active = false;
  bool reverse = false;
  CRGB color;
  uint32_t lastUpdate = 0;
  uint32_t interval;
};

// Constructor for multiple led sections
WipeEffect effects[LEDSECTIONS]; // Store animation state for each segment

void __init__CD77_colorwipe(uint8_t y, CRGB color, uint32_t wait, bool reverse) {
  if (y < LEDSECTIONS) { // Ensure valid index
    effects[y].index = 0;
    effects[y].active = false;
    effects[y].color = color;
    effects[y].lastUpdate = millis();
    effects[y].interval = wait;
    effects[y].reverse = reverse;
  }
}

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
            int y = antenna - 1; // starts from 0
            effects[y].active = true;
            //timer_Start[ledIndex] = millis();
        }
    for (int i = 0; i < LEDSECTIONS; i++) {
        if ((millis() - timer_Start[i] >= timerDuration)) {
            effects[i].active = false;
        }
    }
   
   //Serial.println("yes");
}

void setup_led(){
  // Define FastLED settings
  FastLED.addLeds<chipset, Data_Pin>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  set_max_power_indicator_LED(13);

  // Initialise colorwipe function with WipeEffects atributes
  __init__CD77_colorwipe(0, CRGB::Blue, timerDuration, 0);
  __init__CD77_colorwipe(1, CRGB::Red, timerDuration, 0);
  __init__CD77_colorwipe(2, CRGB::Green, timerDuration, 0);
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

// Non-blocking LED update function
void updateCD77_colorwipe() {
  uint32_t now = millis();

  for (uint8_t y = 0; y < LEDSECTIONS; y++) {
    if (effects[y].active && now - effects[y].lastUpdate >= effects[y].interval) {
      effects[y].lastUpdate = now;

      int8_t i = effects[y].index;

      // Clear previous position
      if ((effects[y].reverse && i < sizearray[y] - 3) || (!effects[y].reverse && i > 2)) {
        ledarray[y][effects[y].reverse ? i + 3 : i - 3] = CRGB::Black;
      }

      // Set the new moving "wipe" with fading effect
      if (i >= 0 && i < sizearray[y]) {
        ledarray[y][i] = effects[y].color;
        if (effects[y].reverse) {
          if (i < sizearray[y] - 1) ledarray[y][i + 1] = effects[y].color - 100;
          if (i < sizearray[y] - 2) ledarray[y][i + 2] = effects[y].color - 200;
        } else {
          if (i > 0) ledarray[y][i - 1] = effects[y].color - 100;
          if (i > 1) ledarray[y][i - 2] = effects[y].color - 200;
        }

        // Move index in correct direction
        effects[y].index += effects[y].reverse ? -1 : 1;
      } else {
        // Reset index to loop the animation and 
        if (effects[y].reverse) {
          if (i < sizearray[y] - 1) ledarray[y][i + 1] = CRGB::Black;
          if (i < sizearray[y] - 2) ledarray[y][i + 2] = CRGB::Black;
        } else {
          if (i > 0) ledarray[y][i - 1] = CRGB::Black;
          if (i > 1) ledarray[y][i - 2] = CRGB::Black;
        }
        effects[y].index = effects[y].reverse ? sizearray[y] - 1 : 0;
      }
    }
  }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    setup_led();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    updateCD77_colorwipe();
    FastLED.show();
}
