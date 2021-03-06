#include <FastLED.h>

#define LED_PIN     3

// Information about the       LED strip itself
#define NUM_LEDS     60
#define CHIPSET     WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  255

#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky

// How many milliseconds to show each temperature before switching
#define DISPLAYTIME 100000
// How many seconds to show black between switches
#define BLACKTIME   3000

uint8_t redList[] = {  0,8,17,26,35,43,52,61,70,79,87,96,105,114,123,131,140,149,158,167,175,184,193,202,211,219,228,237,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,254,253,253,252,252,251,250,250,249,249,248,247,247,246,246,245,245,244,243,243,242,242,241,240,240,239,239,238,237,237,236,236,235,235,234,233,233,232,232,231,230,230,229,229,228,228,227,226,226,225,225,224,223,223,222,222,221,220,220,219,219,218,218,217,216,216,215,215,214,213,213,212,212,211,211,210,209,209,208,208,207,206,206,205,205,204,203,203,202,202,201,201,200,199,199,198,198,197,196,196,195,195,194,193,193,192,192,191,191,190,189,189,188,188,187,186,186,185,185,184,184,183,182,182,181,181,180,179,179,178,178,177,176,176,175,175,174,174,173,172,172,171,171,170,169,169,168,168,167,167,166,165,165,164,164,163,162,162,161,161,160,159,159,158,158,157,157,156,155,155,154,154,153,152,152,151,151,150,150,149,148,147,146,145,144,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128};
uint8_t greenList[] = {  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,13,15,17,19,21,23,26,28,30,32,34,36,39,41,43,45,47,49,52,54,56,58,60,62,65,67,69,71,73,75,78,80,82,84,86,88,91,93,95,97,99,101,104,106,108,110,112,114,117,119,121,123,125,128,128,130,132,135,137,140,142,145,147,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,149,148,147,146,145,144,142,141,140,139,138,137,136,135,134,133,132,131,130,129,128};
uint8_t blueList[] = {  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,5,6,7,9,10,11,12,14,15,16,18,19,20,22,23,24,25,27,28,29,31,32,33,34,36,37,38,40,41,42,44,45,46,47,49,50,51,53,54,55,57,58,59,60,62,63,64,66,67,68,69,71,72,73,75,76,77,79,80,81,82,84,85,86,88,89,90,92,93,94,95,97,98,99,101,102,103,104,106,107,108,110,111,112,114,115,116,117,119,120,121,123,124,125,127,128,129,130,132,133,134,136,137,138,139,141,142,143,145,146,147,149,150,151,152,154,155,156,158,159,160,162,163,164,165,167,168,169,171,172,173,174,176,177,178,180,181,182,184,185,186,187,189,190,191,193,194,195,197,198,199,200,202,203,204,206,207,208,209,211,212,213,215,216,217,219,220,221,222,224,225,226,228,229,230,232,233,234,235,236,237,238,240,241,242,243,244,245,246,247,248,249,250,251,252,253,255};

#define kNumColumns 4
#define kStripHeight 15

uint8_t XY( uint8_t x, uint8_t y)
{
    uint8_t i;
    if( x & 0x1) {
        uint8_t reverseY = (kStripHeight - 1) - y;
        i = (x * kStripHeight) + reverseY;
    } else {
        i = (x * kStripHeight) + y;
    }
    return i;
}

// float bouncing(int time, float speed, float phase) {
//    float pos = time * speed * kStripHeight / 1000 + phase * kStripHeight;
//    pos -= floor(pos / (2*kStripHeight)) * 2*kStripHeight;
//    if (pos > kStripHeight) {
//        pos = 2 * kStripHeight - pos;
//    }
//    return pos;
// }

// float looping(int time, float speed, float phase) {
//    float pos = time * speed * kStripHeight / 1000 + phase * kStripHeight;
//    pos -= floor(pos / (kStripHeight)) * kStripHeight;
//    return pos;
// }


// void loop() {
//     int m = millis();
//     float bluepos = looping(m, -2, 0.2);
//     float greenpos = looping(m, -0.1, 1.8);
//     for (int y = 0; y < kStripHeight; y++) {
//         for (int x = 0; x < kNumColumns; x++) {
//             int addr = XY(x, y);
//             float bluediff = abs(y - bluepos);
//             float bluestr = max(1 - 0.5*bluediff, 0);
//             float greendiff = abs(y - greenpos);
//             float greenstr = max(1 - 0.5*greendiff, 0);
//             leds[addr].r = 0;
//             leds[addr].g = greenstr*255;
//             leds[addr].b = bluestr*255;
//         }
//     }
//   FastLED.show();
//   FastLED.delay(8);
// }

float interpolate(float pos, uint8_t *array, int n) {
    int low = floor(pos);
    int high = low + 1;
    float r = pos - low;
    if (low < 0) low = 0;
    if (low >= n) low = n;
    if (high < low) high = low;
    if (high >= n) high = n;
    return array[low] * (1-r) + array[high]*r;
}

void loop()
{
  int32_t m = millis() % (DISPLAYTIME + BLACKTIME);  
  if (m < BLACKTIME) {
    memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
  } else {
    float pos = (m - BLACKTIME) * 256.0 / DISPLAYTIME;
    float height = 15 - pos / 3;
    for (int h = 0; h < kStripHeight; h++) {
        for (int c = 0; c < kNumColumns; c++) {
            int i = XY(c, h);
            float coef = 1;
            if (h < height) {
                coef = max(0, 1 - 0.2*(height - h));
            }
            leds[i].r = coef * interpolate(pos, redList, 300);
            leds[i].g = coef * interpolate(pos, greenList, 300);
            leds[i].b = coef * interpolate(pos, blueList, 300);
        }
    }
  }
  
  FastLED.show();
  FastLED.delay(8);
}

void setup() {
  Serial.begin(115200);
  delay( 3000 ); // power-up safety delay
  // It's important to set the color correction for your LED strip here,
  // so that colors can be more accurately rendered through the 'temperature' profiles
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  FastLED.setBrightness( BRIGHTNESS );
}


// // How many seconds to show each temperature before switching
// #define DISPLAYTIME 20
// // How many seconds to show black between switches
// #define BLACKTIME   3

// void loop()
// {
//   // draw a generic, no-name rainbow
//   static uint8_t starthue = 0;
//   fill_rainbow( leds + 5, NUM_LEDS - 5, --starthue, 20);

//   // Choose which 'color temperature' profile to enable.
//   uint8_t secs = (millis() / 1000) % (DISPLAYTIME * 2);
//   if( secs < DISPLAYTIME) {
//     FastLED.setTemperature( TEMPERATURE_1 ); // first temperature
//     leds[0] = TEMPERATURE_1; // show indicator pixel
//   } else {
//     FastLED.setTemperature( TEMPERATURE_2 ); // second temperature
//     leds[0] = TEMPERATURE_2; // show indicator pixel
//   }

//   // Black out the LEDs for a few secnds between color changes
//   // to let the eyes and brains adjust
//   if( (secs % DISPLAYTIME) < BLACKTIME) {
//     memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
//   }
  
//   FastLED.show();
//   FastLED.delay(8);
// }

// void setup() {
//   delay( 3000 ); // power-up safety delay
//   // It's important to set the color correction for your LED strip here,
//   // so that colors can be more accurately rendered through the 'temperature' profiles
//   FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
//   FastLED.setBrightness( BRIGHTNESS );
// }
