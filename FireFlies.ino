#include <Adafruit_NeoPixel.h>
#include <FastLED.h>

#define PIN        6
#define WIDTH      12
#define HEIGHT     14
#define NUMPIXELS  (WIDTH * HEIGHT)
#define BRIGHTNESS 50

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// === 3D printed lava lamp mapping ===
int XY(int x, int y) {
    uint8_t physCol = (WIDTH - 1) - x; // reverse columns
    uint16_t i;

    if (physCol & 0x01) {
        i = physCol * HEIGHT + y;            // odd columns go down
    } else {
        i = physCol * HEIGHT + (HEIGHT - 1 - y); // even columns go up
    }
    return i;
}

// --- Firefly struct ---
struct Firefly {
    float x, y;       // position
    float vx, vy;     // velocity
    float brightness; // use float for smooth fading
    bool active;
};

// --- Firefly settings ---
#define NUM_FIREFLIES 8
Firefly fireflies[NUM_FIREFLIES];

// --- Initialize fireflies ---
void initFireflies() {
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        fireflies[i].x = random(0, WIDTH * 100) / 100.0;
        fireflies[i].y = random(0, HEIGHT * 100) / 100.0;
        fireflies[i].vx = random(-5, 6) / 100.0; // slow drift
        fireflies[i].vy = random(-5, 6) / 100.0;
        fireflies[i].brightness = random(128, 255);
        fireflies[i].active = true;
    }
}

// --- Smooth fade all LEDs ---
void fadeAll(float fadeFactor) {
    for (int i = 0; i < NUMPIXELS; i++) {
        uint32_t c = strip.getPixelColor(i);
        float r = float((c >> 16) & 0xFF);
        float g = float((c >> 8) & 0xFF);
        float b = float(c & 0xFF);

        r *= fadeFactor;
        g *= fadeFactor;
        b *= fadeFactor;

        strip.setPixelColor(i, uint8_t(r), uint8_t(g), uint8_t(b));
    }
}

// --- Draw fireflies ---
void drawFireflies() {
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        Firefly &f = fireflies[i];

        int xi = int(f.x + 0.5);
        int yi = int(f.y + 0.5);

        if (xi >= 0 && xi < WIDTH && yi >= 0 && yi < HEIGHT) {
            strip.setPixelColor(XY(xi, yi), strip.Color(uint8_t(f.brightness), uint8_t(f.brightness), 0));
        }

        // Update position
        f.x += f.vx;
        f.y += f.vy;

        // Bounce inside matrix
        if (f.x < 0) { f.x = 0; f.vx *= -1; }
        if (f.x >= WIDTH) { f.x = WIDTH - 1; f.vx *= -1; }
        if (f.y < 0) { f.y = 0; f.vy *= -1; }
        if (f.y >= HEIGHT) { f.y = HEIGHT - 1; f.vy *= -1; }

        // Slow twinkle
        if (random(0, 100) < 2) {
            f.brightness = random(128, 255);
        }
    }
}

void setup() {
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show();
    initFireflies();
}

void loop() {
    fadeAll(0.92);      // smooth, gradual fade (smaller steps)
    drawFireflies();    // move and draw fireflies
    strip.show();
    delay(35);          // faster updates for smoother motion
}
