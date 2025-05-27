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
#define LEDSECTIONS 31


struct WipeEffect {
    int index = 0;
    bool active = false;
    bool reverse = false;
    CRGB color;
    uint32_t lastUpdate = 0;
    uint32_t interval = 200;
};

struct LEDSegment {
    CRGBSet leds;
    uint8_t size;
    bool reverse;
    CRGB color;
    //WipeEffect effect;

    LEDSegment(CRGBSet set, uint8_t s, bool rev, CRGB clr) : leds(set), size(s), reverse(rev), color(clr){}
};

CRGB ledsS1[45];
CRGB ledsS2[105], ledsS3[38], ledsS4[15], ledsS5[37], ledsS6[38], ledsS7[44];

std::vector<LEDSegment> segments = {
    LEDSegment(CRGBSet(ledsS2, 0, 11), 12, true, CRGB::Red), // Middle
    LEDSegment(CRGBSet(ledsS2, 12, 26), 14, false, CRGB::Red),// Segment 1
    LEDSegment(CRGBSet(ledsS2, 29, 32), 3, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS2, 33, 36), 3, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS2, 41, 55), 14, true, CRGB::Purple),
    LEDSegment(CRGBSet(ledsS2, 63, 77), 15, false, CRGB::Red), // Segment 2 (Chained)
    LEDSegment(CRGBSet(ledsS2, 79, 82), 3, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS2, 83, 86), 3, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS2, 90, 104), 14, true, CRGB::Purple),
    
    LEDSegment(CRGBSet(ledsS3, 2, 9), 8, false, CRGB::Purple), // Segment 3
    LEDSegment(CRGBSet(ledsS3, 14, 18), 4, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS3, 19, 23), 4, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS3, 27, 38), 11, true, CRGB::Red),

    LEDSegment(CRGBSet(ledsS5, 3, 8), 6, false, CRGB::Purple), // Segment 4
    LEDSegment(CRGBSet(ledsS5, 13, 17), 5, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS5, 19, 24), 5, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS5, 28, 37), 9, true, CRGB::Red),

    LEDSegment(CRGBSet(ledsS6, 3, 11), 7, false, CRGB::Purple), // Segment 5
    LEDSegment(CRGBSet(ledsS6, 14, 18), 4, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS6, 19, 23), 4, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS6, 26, 38), 11, true, CRGB::Red),

    LEDSegment(CRGBSet(ledsS7, 4, 17), 13, false, CRGB::Purple),  // Segment 6
    LEDSegment(CRGBSet(ledsS7, 20, 23), 3, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS7, 24, 27), 3, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS7, 30, 44), 14, true, CRGB::Red),

    LEDSegment(CRGBSet(ledsS1, 3, 14), 12, false, CRGB::Purple), // Segment 7
    LEDSegment(CRGBSet(ledsS1, 19, 22), 3, false, CRGB::Blue),
    LEDSegment(CRGBSet(ledsS1, 23, 26), 3, false, CRGB::Green),
    LEDSegment(CRGBSet(ledsS1, 30, 45), 15, true, CRGB::Red),

    LEDSegment(CRGBSet(ledsS4, 7, 15), 9, true, CRGB::Red) // Segment 8    
};

// For 8 antennas (0 to 7), define segment indexes controlled by each
const uint8_t antennaToSegments[9][4] = {
    {0, 0, 0, 0},       // Antenna 1: 4 segments
    {9, 10, 11, 12},        // Antenna 2: 4 segments
    {5, 6, 7, 8},           // Antenna 3: 4 segments
    {1, 2, 3, 4},           // Antenna 4: 4 segments
    {0, 29},                
    {25, 26, 27, 28},       // Antenna 5: 2 segments
    {21, 22, 23, 24},       // Antenna 6: 4 segments
    {17, 18, 19, 20},       // Antenna 7: 4 segments
    {13, 14, 15, 16}        // Antenna 8: 4 segments
};

#endif //SET_HEADER_H
