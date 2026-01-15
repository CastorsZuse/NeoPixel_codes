#include "FastLED.h"

// Matrix size
#define NUM_ROWS 14
#define NUM_COLS 12
#define NUM_LEDS NUM_ROWS * NUM_COLS

// LEDs pin
#define DATA_PIN 6

// LED brightness
#define BRIGHTNESS 50

// Define the array of leds
CRGB leds[NUM_LEDS];
CRGB chsvLut[256];
#define bumpsize 18  //bumpsize is bigger leds by 1 witn each side
int8_t bump[bumpsize*bumpsize] = {
  // ... keep bump array as-is ...
};

// === the 3d printed lava lamp mapping ===
// Column-major, bottom-right origin, serpentine columns
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t physCol = (NUM_COLS - 1) - x;  // reverse columns (right → left)
  uint16_t i;

  if (physCol & 0x01) {
    // Odd physical columns go DOWN
    i = (physCol * NUM_ROWS) + y;
  } else {
    // Even physical columns go UP
    i = (physCol * NUM_ROWS) + (NUM_ROWS - 1 - y);
  }

  return i;
}

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  generateCHSVlut();
}

void generateCHSVlut() {
  for (int j = 0; j < 256; j++)
    // chsvLut[j]=CHSV(100,255,j);  //for one color
    chsvLut[j] = HeatColor(j / 1.4);  //256 palette color
}

void Bumpmap(int8_t lightx, int8_t lighty) {
  int col;
  int yindex = bumpsize + 1;
  int8_t vly = lighty;

  for (int y = 0; y < NUM_ROWS; y++) {
    ++vly;
    int8_t vlx = lightx;
    for (int x = 0; x < NUM_COLS; x++) {
      ++vlx;
      int8_t nx = bump[x + yindex + 1] - bump[x + yindex - 1];
      int8_t ny = bump[x + yindex + bumpsize] - bump[x + yindex - bumpsize];
      byte difx = abs8(vlx * 6 - nx);
      byte dify = abs8(vly * 6 - ny);
      int temp = difx * difx + dify * dify;
      col = temp ? 255 - sqrt16(temp) * 3 : 255;
      if (col < 0) col = 0;
      leds[XY(x, y)] = chsvLut[col];
    }
    yindex += bumpsize;
  }
}

void loop() {
#define BumpmapSpeed 30
  int8_t lightX = 1 - beatsin8(BumpmapSpeed, 1, 17, 64);
  int8_t lightY = 1 - beatsin8(BumpmapSpeed - 10, 1, 17);
  Bumpmap(lightX, lightY);
  FastLED.show();
}
