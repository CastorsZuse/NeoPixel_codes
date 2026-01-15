#include <FastLED.h>

#define LED_PIN     6
#define BRIGHTNESS  25
#define LED_TYPE    NEOPIXEL

const uint8_t kMatrixWidth  = 12;
const uint8_t kMatrixHeight = 14;
const bool    kMatrixSerpentineLayout = true;

#define NUM_LEDS ((kMatrixWidth * kMatrixHeight) + 1)
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

CRGB leds[NUM_LEDS];

// Noise coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

uint16_t speed = 1;
uint16_t scale = 50;

uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];
uint8_t lastIndex[kMatrixWidth][kMatrixHeight];

// Palette
//////////////////////////////////////////////////////////////////
// RainbowColors_p, RainbowStripeColors_p, OceanColors_p, 
// CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
///////////////////////////////////////////////////////////////////
CRGBPalette16 currentPalette(PartyColors_p);
uint8_t colorLoop = 0;

// Hue smoothing
static uint16_t hueAccumulator = 0;
static uint8_t ihue = 0;

void setup() {
  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  x = random16();
  y = random16();
  z = random16();

  memset(lastIndex, 0, sizeof(lastIndex));
}

// Fill noise array (UNCHANGED)
void fillnoise8() {
  uint8_t dataSmoothing = 0;
  if (speed < 50) dataSmoothing = 200 - (speed * 4);

  for (int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for (int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;

      uint8_t data = inoise8(x + ioffset, y + joffset, z);
      data = qsub8(data, 64);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing) {
        uint8_t olddata = noise[i][j];
        data = scale8(olddata, dataSmoothing) +
               scale8(data, 256 - dataSmoothing);
      }

      noise[i][j] = data;
    }
  }

  z += speed;
  x += speed / 8;
  y -= speed / 16;
}

// Smooth palette + brightness mapping
void mapNoiseToLEDsUsingPalette() {

  // Slow hue drift (FIX #1)
  hueAccumulator += 16;      // smaller = slower color motion
  ihue = hueAccumulator >> 8;

  for (int i = 0; i < kMatrixWidth; i++) {
    for (int j = 0; j < kMatrixHeight; j++) {

      // Smooth palette index (FIX #2)
      uint8_t index = noise[i][j];
      index = (lastIndex[i][j] * 3 + index) >> 2;
      lastIndex[i][j] = index;

      if (colorLoop) index += ihue;

      // Smooth brightness curve (FIX #3)
      uint8_t bri = noise[i][j];
      bri = scale8(bri, 200);
      bri = qadd8(bri, scale8(bri, bri));

      leds[XY(i, j)] = ColorFromPalette(currentPalette, index, bri);
    }
  }
}

void loop() {
  fillnoise8();
  mapNoiseToLEDsUsingPalette();
  FastLED.show();
}

// Column-major serpentine mapping (bottom-right origin)
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t physCol = (kMatrixWidth - 1) - x;
  uint16_t i;

  if (physCol & 0x01) {
    i = (physCol * kMatrixHeight) + y;
  } else {
    i = (physCol * kMatrixHeight) + (kMatrixHeight - 1 - y);
  }
  return i;
}

// ===== Optional palette helpers (unchanged) =====

void SetupRandomPalette() {
  currentPalette = CRGBPalette16(
    CHSV(random8(), 255, 32),
    CHSV(random8(), 255, 255),
    CHSV(random8(), 128, 255),
    CHSV(random8(), 255, 255)
  );
}

void SetupBlackAndWhiteStripedPalette() {
  fill_solid(currentPalette, 16, CRGB::Black);
  currentPalette[0]  = CRGB::White;
  currentPalette[4]  = CRGB::White;
  currentPalette[8]  = CRGB::White;
  currentPalette[12] = CRGB::White;
}

void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green  = CHSV(HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;

  currentPalette = CRGBPalette16(
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black
  );
}
