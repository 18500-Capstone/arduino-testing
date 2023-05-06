#include <FastLED.h> 

//LED Set up 
#define NUM_LEDS 60 //change to 60
#define LED_PIN 11      //(blue) on vest
#define CLOCK_PIN 12   //(green) on vest
#define COLOR_ORDER BGR  //BLUE, GREEN, RED

CRGB leds[NUM_LEDS];
uint8_t colorIndex[NUM_LEDS];
uint8_t patternCounter = 0;
uint8_t paletteIndex = 0; 

// Gradient palette "cascade_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_seis.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 40 bytes of program space.

DEFINE_GRADIENT_PALETTE( cascade_gp ) {
    0,  88,  0,  0,
   28, 255,  0,  0,
   56, 255, 22,  0,
   85, 255,104,  0,
  113, 255,255,  0,
  141, 255,255,  0,
  169,  17,255,  1,
  198,   0,223, 31,
  226,   0, 19,255,
  255,   0,  0,147};


CRGBPalette16 myPal = cascade_gp;  

void setup() {
  //Serial.begin(9600);
  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(100);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show(); 

  //Fill the colorIndex array with random numbers
  for (int i = 0; i < NUM_LEDS; i++) {
    colorIndex[i] = random8();
  }
}

//rainbow light show for forceful jump
void FJ_LightShow(){
   fill_palette(leds, NUM_LEDS, paletteIndex, 255 / 60, myPal, 255, LINEARBLEND);
   EVERY_N_MILLISECONDS(10){
    paletteIndex++;
  }
  FastLED.show(); 
}

//pixel moving fast
void moveFast(){
  //blue light pixel moving around 
  uint8_t sinBeat = beatsin8(22, 0, NUM_LEDS - 1, 0, 0);
  leds[sinBeat] = CRGB::Blue;

  fadeToBlackBy(leds, NUM_LEDS, 10);
  EVERY_N_MILLISECONDS(10){
    Serial.println(sinBeat);
  }
  FastLED.show();
  
}

//flash red when in low health 
void lowHealth_LightShow(){
  EVERY_N_MILLISECONDS(200){
     fill_solid(leds, NUM_LEDS, CRGB::Black);
     FastLED.show();
  }
  EVERY_N_MILLISECONDS(400){
   fill_solid(leds, NUM_LEDS, CRGB::Red); //when player dies, lights stays red
   FastLED.show();
  }
}

//blue light pixel moving around 
void hitSmall_LightShow(){
  uint8_t sinBeat = beatsin8(22, 0, NUM_LEDS - 1, 0, 0);
  leds[sinBeat] = CRGB(58, 156, 223);
  fadeToBlackBy(leds, NUM_LEDS, 30); //30 is hit small, 5 is hit large
  FastLED.show();  
}

//purple light pixel moving around 
void hitLarge_LightShow(){
  uint8_t sinBeat = beatsin8(22, 0, NUM_LEDS - 1, 0, 0);
  leds[sinBeat] = CRGB::Magenta;
  fadeToBlackBy(leds, NUM_LEDS, 5); //20 is hit small, 5 is hit large
  FastLED.show();
  
}

void loop() {
  switch(patternCounter){
    case 0: 
      FJ_LightShow();
      break;
    case 1: 
      hitSmall_LightShow(); 
      break;
    case 2: 
      hitLarge_LightShow();
      break;
    case 3:
      lowHealth_LightShow();
      break; 
  }

  EVERY_N_SECONDS(5){
    nextPattern();
  }
  FastLED.show();
}

void nextPattern(){
  patternCounter = (patternCounter + 1) % 4;
}
