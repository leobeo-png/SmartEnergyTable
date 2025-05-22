#include "SET_Header.h"

unsigned long timerUpdate;
unsigned long ledSpeed = 200;

// WiFi credentials
const char* ssid = "Pepper_C1-MUX-88B704";
const char* password = "";

// MQTT broker settings
const char* mqtt_server = "192.168.100.2";
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt_user";
const char* mqtt_password = "smartenergytable";
const char* mqtt_topic = "esp/test";

WiFiClient espClient;
PubSubClient client(espClient);

const unsigned long timerDuration = 400;
bool antennaFlag = false;
int antenna;
unsigned long antennaLastActive[LEDSECTIONS] = {0}; 
bool antennaActive[LEDSECTIONS] = {false};

// Constructor for multiple led sections
WipeEffect effects[LEDSECTIONS]; // Store animation state for each segment

void __init__CD77_colorwipe(uint8_t y, CRGB color, uint32_t wait, bool reverse) {
  effects[y].index = 0;
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
     
    //antennaLastActive[antenna] = millis();
    //antennaActive[antenna] = true;  // Mark it as active
    /*
    // Turn on LEDs for the corresponding section
    if (antenna >= 0 && antenna < segments.size()) {
      segments[antenna].effect.active = true;
      antennaLastActive[antenna] = millis();
      antennaActive[antenna] = true;
      Serial.println("Segment activated.");
    }
    */
    for (int i = 0; i < 5; i++) {
        uint8_t segmentIndex = antennaToSegments[antenna][i];
        effects[segmentIndex].active = true;
        effects[segmentIndex].index = 0;  // Optionally reset effect position
        antennaLastActive[segmentIndex] = millis();
        antennaActive[segmentIndex] = true;
    }
    Serial.print("Antenna ");
    Serial.print(antenna);
    Serial.println(" activated.");
    
}

void setup_led(){
  // Define FastLED settings
  FastLED.addLeds<CHIPSET, Data_PinS1>(ledsS1, 105);
  FastLED.addLeds<CHIPSET, Data_PinS2>(ledsS2, 20);
  FastLED.addLeds<CHIPSET, Data_PinS3>(ledsS3, 20);
  FastLED.addLeds<CHIPSET, Data_PinS4>(ledsS4, 20);
  FastLED.addLeds<CHIPSET, Data_PinS5>(ledsS5, 20);
  FastLED.addLeds<CHIPSET, Data_PinS6>(ledsS6, 20);
  FastLED.addLeds<CHIPSET, Data_PinS7>(ledsS7, 20);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  set_max_power_indicator_LED(13);

  // Initialise colorwipe function with WipeEffects atributes
  for (int i = 0; i < LEDSECTIONS; i++){
  __init__CD77_colorwipe(i, CRGB::Red, ledSpeed, 0);
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
    auto& seg = segments[i];
    auto& eff = seg.effect;

    if (eff.active && now - eff.lastUpdate >= eff.interval) {
      eff.lastUpdate = now;
      uint8_t idx = eff.index;

      if ((eff.reverse && idx < seg.size - 3) || (!eff.reverse && idx > 2)) {
        seg.leds[eff.reverse ? idx + 3 : idx - 3] = CRGB::Black;
      }

      if (idx < seg.size) {
        seg.leds[idx] = eff.color;
        if (eff.reverse) {
          if (idx + 1 < seg.size) seg.leds[idx + 1] = eff.color - 100;
          if (idx + 2 < seg.size) seg.leds[idx + 2] = eff.color - 200;
        } else {
          if (idx > 0) seg.leds[idx - 1] = eff.color - 100;
          if (idx > 1) seg.leds[idx - 2] = eff.color - 200;
        }
        eff.index += eff.reverse ? -1 : 1;
      } else {
        eff.index = eff.reverse ? seg.size - 1 : 0;
      }
    }

    if (antennaActive[i] && (now - antennaLastActive[i] >= timerDuration)) {
      eff.active = false;
      fill_solid(seg.leds, seg.size, CRGB::Black);
      antennaActive[i] = false;
    }
  }
  FastLED.show();
}
 /* 
  uint32_t now = millis();

  for (uint8_t y = 0; y < LEDSECTIONS; y++) {
    if (effects[y].active && now - effects[y].lastUpdate >= effects[y].interval) {
      effects[y].lastUpdate = now;

      uint8_t i = effects[y].index;

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

  for (uint8_t y = 1; y < LEDSECTIONS; y++) {
    if (antennaActive[y] && (now - antennaLastActive[y] >= timerDuration)) {
      Serial.print("Antenna ");
      Serial.print(y);
      Serial.println(" has been inactive, turning off LEDs.");

      effects[y].active = false;  // Disable this antenna's LED effect
      fill_solid(ledarray[y], sizearray[y], CRGB::Black);  // Turn off only this section
      antennaActive[y] = false;  // Mark it as inactive
      }
    }
  FastLED.show();
}
*/
void setup() {
    Serial.begin(115200);
    setup_wifi();
    setup_led();
    effects[0].active = true;
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    updateCD77_colorwipe();
}
