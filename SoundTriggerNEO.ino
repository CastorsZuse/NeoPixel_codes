// CUT AND PASTE THESE LINES ABOVE THE "VOID SETUP" CHECK FOR DUPLICATE LIBRARIES IN YOUR CODE
// "NUM_LEDS" AND "DATA_PIN" MAY NEED TO BE RENAMED DEPENDING ON YOUR CODES NAMES.

#include <Adafruit_NeoPixel.h>
#define FIREPIN      4  // Fire button
int buttonState = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
boolean oldState = HIGH;


// CUT AND PASTE THESE LINES IN THE "VOID SETUP" 

// void setup() {
  strip.begin();
  strip.show();
  pinMode(FIREPIN, INPUT_PULLUP);


//REPLACE YOUR "VOID LOOP" WITH THESE LINES
//MOVE YOUR VOID LOOP TO THE END OF THE CODE, AFTER ALL THE FUNCTIONS (for compiling reasons)
//COPY AND PASTE YOUR FUNCTION CALL INTO THE AREA INDICATD AFTER  "if (buttonState == LOW) {"
//DONT BE SHOCKED IF IT DOESNT COMPILE AT FIRST. SOME NAMES MAY NEED TO BE CHANGED TO MATCH BOTH CODES

// void loop() { 
  uint8_t  i;
  boolean newState = digitalRead(FIREPIN);  
  //Button switch
  buttonState = digitalRead(FIREPIN);
    if((newState == LOW) && (oldState == HIGH)) {
    // Short delay to debounce button.
    delay(5);
    // Check if button is still low after debounce.
    newState = digitalRead(FIREPIN);
    if(newState == LOW) {
  // FUNCTION CALL ////////////////////////////////////////////////////////////////////////    


  /////////////////////////////////////////////////////////////////////////////////////////
  } 
  
  else {
     strip.setPixelColor(i, 0,0,0); //Button not pressed, turn off pixels
     strip.show(); //Show no pixels
  }
}

