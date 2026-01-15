#include <FastLED.h>

#define WIDTH      12
#define HEIGHT     14
#define NUM_LEDS   (WIDTH * HEIGHT)
#define DATA_PIN   6

CRGB leds[NUM_LEDS];

// === the 3d printed lava lamp mapping ===
// Column-major, bottom-right origin, serpentine columns
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t physCol = (WIDTH - 1) - x;  // reverse columns (right → left)
  uint16_t i;

  if (physCol & 0x01) {
    // Odd physical columns go DOWN
    i = (physCol * HEIGHT) + y;
  } else {
    // Even physical columns go UP
    i = (physCol * HEIGHT) + (HEIGHT - 1 - y);
  }

  return i;
}

// 🌈 CHOOSE YOUR PALETTE HERE
CRGBPalette16 currentPalette = PartyColors_p;
/*
  RainbowColors_p
  RainbowStripeColors_p
  PartyColors_p
  OceanColors_p
  CloudColors_p
  LavaColors_p
  HeatColors_p
  ForestColors_p
*/

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.setBrightness(100);
}

void loop() {
  float time = millis() / 1000.0;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {

      float waveX = sin((x * 0.5) + (time * 2.0));
      float waveY = cos((y * 0.3) + (time * 1.5));
      float combined = (waveX + waveY) * 0.5;

      uint8_t brightness = constrain((combined + 1.0) * 128, 0, 255);
      uint8_t colorIndex = (combined + 1.0) * 127;

      leds[XY(x, y)] = ColorFromPalette(
        currentPalette,
        colorIndex,
        brightness,
        LINEARBLEND
      );
    }
  }

  FastLED.show();
  delay(20);
}
