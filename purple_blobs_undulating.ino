#include <Adafruit_NeoPixel.h>

#define PIN        6
#define WIDTH      5
#define HEIGHT     10
#define NUMPIXELS  (WIDTH * HEIGHT)

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Simulated Perlin-like noise
float noise(float x) {
  return (sin(x) + sin(x * 1.3) + sin(x * 0.7)) / 3.0;
}

int xyToIndex(int x, int y) {
  if (x % 2 == 0)
    return x * HEIGHT + y;
  else
    return x * HEIGHT + (HEIGHT - 1 - y);
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
  t += 0.03;

  strip.clear();

  const int numBlobs = 2;
  float baseRadius = 3.45;

  // Purple outer blobs
  uint32_t purpleColors[numBlobs] = {
    strip.Color(180, 0, 255),  // Bright violet
    strip.Color(100, 0, 200)   // Deep purple
  };

  // Intense green core
  uint32_t greenColor = strip.Color(0, 255, 100);  // Bright green

  for (int i = 0; i < numBlobs; i++) {
    float fx = WIDTH / 2.0 + noise(t + i * 10.0) * WIDTH / 3.0;
    float fy = HEIGHT / 2.0 + noise(t + i * 10.0 + 50) * HEIGHT / 3.0;

    float pulse = 0.25 * sin(t * 2.0 + i * 5.0);  // Undulation
    float outerRadius = baseRadius + pulse;
    float innerRadius = outerRadius * 0.5;  // Green core size

    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        float dx = fx - x;
        float dy = fy - y;
        float dist = sqrt(dx * dx + dy * dy);

        if (dist < outerRadius) {
          float intensity = pow((outerRadius - dist) / outerRadius, 2.0);
          addColor(x, y, scaleColor(purpleColors[i], intensity));

          // Green core (intense)
          if (dist < innerRadius) {
            float greenIntensity = pow((innerRadius - dist) / innerRadius, 2.0) * 1.5;
            greenIntensity = min(1.0, greenIntensity);  // Clamp to avoid over-bright
            addColor(x, y, scaleColor(greenColor, greenIntensity));
          }
        }
      }
    }
  }

  strip.show();
  delay(30);
}
