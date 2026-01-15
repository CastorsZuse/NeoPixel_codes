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

// --- Forward declare Spark struct for compiler ---
struct Spark;

// --- Function prototypes ---
void drawSpark(Spark &s);
void spawnSpark(int idx);
void updateSparks();
void fadeAll(uint8_t fadeAmount);

// --- Lightning bolt struct ---
struct Spark {
    int x, y;
    int dx, dy;       // movement direction
    int life;         // lifetime
    bool active;
};

// --- Lightning sparks ---
#define MAX_SPARKS 10
Spark sparks[MAX_SPARKS];

// --- Palette selection ---
// <<< SET THE PALETTE YOU WANT HERE >>>
CRGBPalette16 currentPalette = ForestColors_p; // <-- choose one palette here
// Options: RainbowColors_p, RainbowStripeColors_p, OceanColors_p, 
// CloudColors_p, LavaColors_p, ForestColors_p, PartyColors_p

// --- Initialize random spark ---
void spawnSpark(int idx) {
    sparks[idx].x = random(0, WIDTH);
    sparks[idx].y = random(0, HEIGHT);
    sparks[idx].dx = random(-1, 2);
    sparks[idx].dy = random(-1, 2);
    sparks[idx].life = random(3, 7);
    sparks[idx].active = true;
}

// --- Draw a spark ---
void drawSpark(Spark &s) {
    if (s.x >= 0 && s.x < WIDTH && s.y >= 0 && s.y < HEIGHT) {
        uint8_t colorIndex = random8(); // random hue in current palette
        CRGB c = ColorFromPalette(currentPalette, colorIndex, 255);
        strip.setPixelColor(XY(s.x, s.y), strip.Color(c.r, c.g, c.b));
    }
}

// --- Move sparks ---
void updateSparks() {
    for (int i = 0; i < MAX_SPARKS; i++) {
        if (!sparks[i].active || sparks[i].life <= 0) {
            if (random(0, 100) < 10) spawnSpark(i);
            continue;
        }

        drawSpark(sparks[i]);

        sparks[i].x += sparks[i].dx;
        sparks[i].y += sparks[i].dy;
        sparks[i].life--;

        // Cylindrical horizontal wrap
        if (sparks[i].x < 0) sparks[i].x = WIDTH - 1;
        if (sparks[i].x >= WIDTH) sparks[i].x = 0;

        // Bounce vertically
        if (sparks[i].y < 0) sparks[i].y = 0;
        if (sparks[i].y >= HEIGHT) sparks[i].y = HEIGHT - 1;
    }
}

// --- Fade all LEDs slightly ---
void fadeAll(uint8_t fadeAmount) {
    for (int i = 0; i < NUMPIXELS; i++) {
        uint32_t c = strip.getPixelColor(i);
        uint8_t r = (c >> 16) & 0xFF;
        uint8_t g = (c >> 8) & 0xFF;
        uint8_t b = c & 0xFF;

        r = r * (255 - fadeAmount) / 255;
        g = g * (255 - fadeAmount) / 255;
        b = b * (255 - fadeAmount) / 255;

        strip.setPixelColor(i, r, g, b);
    }
}

void setup() {
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show();

    for (int i = 0; i < MAX_SPARKS; i++) sparks[i].active = false;
}

void loop() {
    fadeAll(50);      // fade previous arcs quickly
    updateSparks();   // move sparks
    strip.show();
    delay(50);        // speed of lightning flashes
}
