#include "SET_Header.h"

unsigned long timerUpdate;
unsigned long ledSpeed = 70;

// WiFi credentials
const char* ssid = "Pepper_C1-MUX-88B704";
const char* password = "";

// MQTT broker settings
const char* mqtt_server = "192.168.100.5";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "smartenergytable";
const char* mqtt_topic = "esp/test";

WiFiClient espClient;
PubSubClient client(espClient);

const unsigned long timerDuration = 1000;
bool antennaFlag = false;
int antenna;
unsigned long antennaLastActive[LEDSECTIONS] = {0}; 
bool antennaActive[LEDSECTIONS] = {false};

// Constructor for multiple led sections
WipeEffect effects[LEDSECTIONS]; // Store animation state for each segment

void __init__CD77_colorwipe(uint8_t y, CRGB color, uint32_t wait, bool reverse) {
  effects[y].index = reverse ? segments[y].size - 1 : 0;
  effects[y].active = false;
  effects[y].color = color;
  effects[y].lastUpdate = millis();
  effects[y].interval = wait;
  effects[y].reverse = reverse;
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
  int separatorIndex = message.indexOf(':');  
  String command = message.substring(0, separatorIndex);
  antenna = message.substring(separatorIndex + 1).toInt();
  if (antenna <= 0 || antenna > 8) return;  // invalid antenna index
  
  // Determine how many segments are valid for this antenna
  uint8_t segmentCount = (antenna == 4) ? 2 : 4;
  
  for (int i = 0; i < segmentCount; i++) {
    uint8_t segmentIndex = antennaToSegments[antenna][i];
    if (segmentIndex >= segments.size()) continue;

    // Only reset if animation is not running
    if (!effects[segmentIndex].active) {
        effects[segmentIndex].active = true;
        effects[segmentIndex].index = effects[segmentIndex].reverse ? segments[segmentIndex].size - 1 : 0;
    }

    antennaLastActive[segmentIndex] = millis();
    antennaActive[segmentIndex] = true;
    Serial.printf("Activated segment %d for antenna %d\n", segmentIndex, antenna);
  }
}

void setup_led(){
  // Define FastLED settings
  FastLED.addLeds<CHIPSET, Data_PinS1>(ledsS1, 45);
  FastLED.addLeds<CHIPSET, Data_PinS2>(ledsS2, 105);
  FastLED.addLeds<CHIPSET, Data_PinS3>(ledsS3, 38);
  FastLED.addLeds<CHIPSET, Data_PinS4>(ledsS4, 15);
  FastLED.addLeds<CHIPSET, Data_PinS5>(ledsS5, 37);
  FastLED.addLeds<CHIPSET, Data_PinS6>(ledsS6, 38);
  FastLED.addLeds<CHIPSET, Data_PinS7>(ledsS7, 44);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  set_max_power_indicator_LED(13);

  // Initialise colorwipe function with WipeEffects atributes
  for (int i = 0; i <= LEDSECTIONS; i++){
    __init__CD77_colorwipe(i, segments[i].color, ledSpeed, segments[i].reverse);
  }
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

  for (int i = 0; i < segments.size(); i++) {
    auto& eff = effects[i];
    auto& seg = segments[i];

    if (eff.active && now - eff.lastUpdate >= eff.interval) {
      eff.lastUpdate = now;

      // Clear trailing pixels behind current index
      for (int t = 1; t <= 3; ++t) {
        int clearPos = eff.reverse ? eff.index + t : eff.index - t;
        if (clearPos >= 0 && clearPos < seg.size) {
          seg.leds[clearPos] = CRGB::Black;
        }
      }
      // Draw head + trail
      if (eff.index >= 0 && eff.index < seg.size) {
        seg.leds[eff.index] = eff.color;

        for (int t = 1; t <= 2; ++t) {
          int trailPos = eff.reverse ? eff.index + t : eff.index - t;
          if (trailPos >= 0 && trailPos < seg.size) {
            seg.leds[trailPos] = eff.color;
            seg.leds[trailPos].fadeToBlackBy(t * 100);
          }
        }
      }
      // Advance index
      eff.index += eff.reverse ? -1 : 1;

      if (eff.reverse) {
        if (eff.index < -3) {
          eff.index = seg.size - 1;
        }
      } else {
        if (eff.index > seg.size + 2) {
          eff.index = 0;
        }
      }
    }
    // If antenna is not active > deactive leds
    if (antennaActive[i] && (now - antennaLastActive[i] >= timerDuration)) {
      eff.active = false;
      fill_solid(seg.leds, seg.size, CRGB::Black);
      antennaActive[i] = false;
    } 
  }
  FastLED.show();
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    setup_led();
    FastLED.show();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    updateCD77_colorwipe();
    //delay(10); // Smooth timing
    FastLED.show();
}