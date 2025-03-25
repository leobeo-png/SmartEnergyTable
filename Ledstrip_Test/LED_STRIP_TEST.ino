#include "FastLED.h"

#define NUM_LEDS 120
#define Data_Pin 25
#define chipset WS2812B
#define BRIGHTNESS 50
#define LEDSECTIONS 3

CRGB rawleds[NUM_LEDS];
CRGBSet leds(rawleds, NUM_LEDS);
CRGBSet leds1(leds(0, 7));
CRGBSet leds2(leds(8, 15));
CRGBSet leds3(leds(16, 23));

struct CRGB *ledarray[] = {leds1, leds2, leds3}; // An array of the CRGBSet arrays
uint8_t sizearray[] = {8, 8, 8}; // Size of the above arrays

struct WipeEffect {
  uint8_t index = 0;
  bool active = false;
  bool reverse = false;
  CRGB color;
  uint32_t lastUpdate = 0;
  uint32_t interval;
};

WipeEffect effects[LEDSECTIONS]; // Store animation state for each segment

void setup() {
  delay(2000); // power-up safety delay
  FastLED.addLeds<chipset, Data_Pin>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);
  set_max_power_indicator_LED(13);

  // Initialize wipe effects
  __init__CD77_colorwipe(0, CRGB::Blue, 100, 1);
  __init__CD77_colorwipe(1, CRGB::Red, 100, 0);
  __init__CD77_colorwipe(2, CRGB::Green, 100, 1);

}

void loop() {
  updateCD77_colorwipe();
  FastLED.show();
}

// Start a new wipe animation
void __init__CD77_colorwipe(uint8_t y, CRGB color, uint32_t wait, bool reverse) {
  if (y < LEDSECTIONS) { // Ensure valid index
    effects[y].index = 0;
    effects[y].active = true;
    effects[y].color = color;
    effects[y].lastUpdate = millis();
    effects[y].interval = wait;
    effects[y].reverse = reverse;
  }
}

// Non-blocking function to update wipes
void updateCD77_colorwipe() {
  uint32_t now = millis();

  for (uint8_t y = 0; y < LEDSECTIONS; y++) {
    if (effects[y].active && now - effects[y].lastUpdate >= effects[y].interval) {
      effects[y].lastUpdate = now;

      int8_t i = effects[y].index; // Allow negative values for reversing

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