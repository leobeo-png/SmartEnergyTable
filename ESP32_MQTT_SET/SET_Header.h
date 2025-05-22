//
// Created by Lou on 20-5-2025.
//

#ifndef SET_HEADER_H
#define SET_HEADER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>

// LED definintions
#define Data_PinS1 25
#define Data_PinS2 26
#define Data_PinS3 27
#define Data_PinS4 33
#define Data_PinS5 12
#define Data_PinS6 14
#define Data_PinS7 32

#define CHIPSET WS2812B
#define BRIGHTNESS 50
#define LEDSECTIONS 15


struct WipeEffect {
    uint8_t index = 0;
    bool active = false;
    bool reverse = false;
    CRGB color;
    uint32_t lastUpdate = 0;
    uint32_t interval = 200;
};

struct LEDSegment {
    CRGBSet leds;
    uint8_t size;
    WipeEffect effect;

    LEDSegment(CRGBSet set, uint8_t s) : leds(set), size(s) {}
};

CRGB ledsS1[105];
CRGB ledsS2[20], ledsS3[20], ledsS4[20], ledsS5[20], ledsS6[20], ledsS7[20];

std::vector<LEDSegment> segments = {
    LEDSegment(CRGBSet(ledsS1, 0, 11), 12),
    LEDSegment(CRGBSet(ledsS1, 12, 26), 15),
    LEDSegment(CRGBSet(ledsS1, 29, 32), 4),
    LEDSegment(CRGBSet(ledsS1, 33, 36), 4),
    LEDSegment(CRGBSet(ledsS1, 41, 55), 15),
    LEDSegment(CRGBSet(ledsS1, 63, 77), 15),
    LEDSegment(CRGBSet(ledsS1, 79, 82), 4),
    LEDSegment(CRGBSet(ledsS1, 83, 86), 4),
    LEDSegment(CRGBSet(ledsS1, 90, 104), 15),
    LEDSegment(CRGBSet(ledsS2, 0, 19), 20),
    LEDSegment(CRGBSet(ledsS3, 0, 19), 20),
    LEDSegment(CRGBSet(ledsS4, 0, 19), 20),
    LEDSegment(CRGBSet(ledsS5, 0, 19), 20),
    LEDSegment(CRGBSet(ledsS6, 0, 19), 20),
    LEDSegment(CRGBSet(ledsS7, 0, 19), 20)
};

// For 8 antennas (0 to 7), define segment indexes controlled by each
const uint8_t antennaToSegments[8][4] = {
    {1, 2, 3, 4},       // Antenna 0: 4 segments
    {5, 6, 7, 8},       // Antenna 1: 4 segments
    {9, 10, 11, 12},    // Antenna 2: 4 segments
    {13, 14, 15, 16},   // Antenna 3: 4 segments
    {17, 18, 19, 20},   // Antenna 4: 4 segments
    {21, 22, 23, 24},   // Antenna 5: 4 segments
    {25, 26, 27, 28},   // Antenna 6: 4 segments
    {29}                // Antenna 7: 1 segment
};

#endif //SET_HEADER_H
