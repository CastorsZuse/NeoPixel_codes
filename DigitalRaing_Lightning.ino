#include <Adafruit_NeoPixel.h>

#define PIN         6
#define WIDTH       12
#define HEIGHT      14
#define NUMPIXELS   (WIDTH * HEIGHT)
#define BRIGHTNESS  50

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// === 3D printed lava lamp mapping ===
int XY(int x, int y) {
    uint8_t physCol = (WIDTH - 1) - x; // reverse columns (right → left)
    uint16_t i;

    if (physCol & 0x01) {
        // Odd columns go DOWN
        i = physCol * HEIGHT + y;
    } else {
        // Even columns go UP
        i = physCol * HEIGHT + (HEIGHT - 1 - y);
    }
    return i;
}

// --- Drop structure ---
struct Drop {
    int headY = -1;     // current head position
    int originY = 0;    // starting Y position of drop
    int tailLength;
};

Drop drops[WIDTH];

// Timing control
int frameCounter = 0;
const int updateInterval = 2;  // slower rain

void setup() {
    strip.begin();
    strip.setBrightness(BRIGHTNESS);
    strip.show();

    for (int x = 0; x < WIDTH; x++) {
        drops[x].originY = random(-HEIGHT / 2, HEIGHT);
        drops[x].headY = drops[x].originY;
        drops[x].tailLength = random(3, 5);
    }
}

// --- Smooth fade ---
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

// --- Fade from green to purple ---
uint8_t lerp(uint8_t start, uint8_t end, float t) {
    return (1.0 - t) * start + t * end;
}

uint32_t fadeToPurple(float t) {
    uint8_t r = lerp(0, 100, t);
    uint8_t g = lerp(255, 0, t);
    uint8_t b = lerp(0, 100, t);
    return strip.Color(r, g, b);
}

// --- Lightning flash ---
void lightningFlash() {
    int startX = random(0, WIDTH - 1);
    int endX   = min(startX + random(1, 2), WIDTH - 1);

    // Step 1: Yellow flash
    for (int x = startX; x <= endX; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            strip.setPixelColor(XY(x, y), strip.Color(255, 255, 0));
        }
    }
    strip.show();
    delay(30);

    // Step 2: White cooldown
    for (int x = startX; x <= endX; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            strip.setPixelColor(XY(x, y), strip.Color(255, 255, 255));
        }
    }
    strip.show();
    delay(20);

    // Step 3: Fast fade
    fadeAll(100);
    strip.show();
}

void loop() {
    fadeAll(6);  // smooth trail fading

    // Occasionally flash lightning
    if (random(0, 500) < 5) {
        lightningFlash();
    }

    frameCounter++;
    if (frameCounter >= updateInterval) {
        frameCounter = 0;

        for (int x = 0; x < WIDTH; x++) {
            drops[x].headY++;

            // Bright green head
            if (drops[x].headY >= 0 && drops[x].headY < HEIGHT) {
                strip.setPixelColor(XY(x, drops[x].headY), strip.Color(0, 255, 0));
            }

            // Tail below the head, starting from max(originY, headY - tailLength)
            int tailStart = max(drops[x].originY, drops[x].headY - drops[x].tailLength);
            for (int tailY = tailStart; tailY < drops[x].headY; tailY++) {
                if (tailY >= 0 && tailY < HEIGHT) {
                    float fadeFactor = (float)(drops[x].headY - tailY) / drops[x].tailLength;
                    strip.setPixelColor(XY(x, tailY), fadeToPurple(fadeFactor));
                }
            }

            // Reset when fully off-screen
            if (drops[x].headY - drops[x].tailLength > HEIGHT) {
                drops[x].originY = random(-HEIGHT / 2, HEIGHT);
                drops[x].headY = drops[x].originY;
                drops[x].tailLength = random(3, 6);
            }
        }
    }

    strip.show();
    delay(30);  // overall speed
}
