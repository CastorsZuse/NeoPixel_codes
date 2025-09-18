#include <Adafruit_NeoPixel.h>

#define PIN        6
#define WIDTH      10
#define HEIGHT     5
#define NUMPIXELS  (WIDTH * HEIGHT)

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Simulated Perlin-like noise
float noise(float x) {
  return (sin(x) + sin(x * 1.3) + sin(x * 0.7)) / 3.0;
}

// XY → LED index mapping with zig-zag layout and offset by 24
int xyToIndex(int x, int y) {
  int i;
  if (x % 2 == 0)
    i = x * HEIGHT + y;
  else
    i = x * HEIGHT + (HEIGHT - 1 - y);

  // Shift mapping so LED 0 is now at physical LED 24
  i = (i + 24) % NUMPIXELS;

  return i;
}

uint32_t scaleColor(uint32_t color, float scale) {
  byte r = min(255, ((color >> 16) & 0xFF) * scale);
  byte g = min(255, ((color >> 8) & 0xFF) * scale);
  byte b = min(255, (color & 0xFF) * scale);
  return strip.Color(r, g, b);
}

void addColor(int x, int y, uint32_t color) {
  int i = xyToIndex(x, y);
  uint32_t existing = strip.getPixelColor(i);

  byte r = min(255, ((existing >> 16) & 0xFF) + ((color >> 16) & 0xFF));
  byte g = min(255, ((existing >> 8) & 0xFF) + ((color >> 8) & 0xFF));
  byte b = min(255, (existing & 0xFF) + (color & 0xFF));

  strip.setPixelColor(i, r, g, b);
}

void setup() {
  strip.begin();
  strip.clear();
  strip.show();
}

void loop() {
  static float t = 0.0;
  t += 0.02;   // smooth and slower motion

  // Dim background (dark)
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      strip.setPixelColor(xyToIndex(x,y), strip.Color(3, 0, 5));  
    }
  }

  for (int i = 0; i < 3; i++) {
    // Spread blobs farther apart
    float fx = WIDTH / 2.0 + noise(t + i * 10.0) * WIDTH / 2.0;
    float fy = HEIGHT / 2.0 + noise(t + i * 10.0 + 50) * HEIGHT / 2.0;

    // Rainbow color cycling (hue depends on time + blob index)
    uint16_t hue = (uint16_t)((t * 40) + i * 20000) % 65535; 
    uint32_t color = strip.gamma32(strip.ColorHSV(hue, 255, 255));

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        float dx = fx - x;
        float dy = fy - y;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 3.0) {
          float intensity = pow((3.0 - dist) / 3.0, 2.0);
          addColor(x, y, scaleColor(color, intensity));
        }
      }
    }
  }

  strip.show();
  delay(38);   // 25% slower frame update
}
