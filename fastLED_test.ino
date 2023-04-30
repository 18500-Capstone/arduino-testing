#include <FastLED.h> 

//LED Set up 
#define NUM_LEDS 60 //change to 60
#define LED_PIN 11      //(blue) on vest
#define CLOCK_PIN 12   //(green) on vest
#define COLOR_ORDER BGR   //BLUE, GREEN, BLUE

CRGB leds[NUM_LEDS];

uint8_t paletteIndex = 0; 

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ){
  0,   120,   0,   0,       
  22, 179, 22,   0,     
  51, 255, 104, 0,  
  85, 167, 22, 10,
  135, 100, 0, 103,
  190, 16, 0, 130,    
  255, 0, 0, 160     
};

// Gradient palette "cascadeBackup_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 12 bytes of program space.

DEFINE_GRADIENT_PALETTE( cascadeBackup_gp ) {
    0, 227,101,  3,
  117, 194, 18, 19,
  255,  92,  8,192};

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

  //FJ_LightShow();
  
}

//rainbow light show for forceful jump
void FJ_LightShow(){
   fill_palette(leds, NUM_LEDS, paletteIndex, 255 / 60, myPal, 255, LINEARBLEND);
   EVERY_N_MILLISECONDS(10){
    paletteIndex++;
  }
  FastLED.show(); 
}

void loop() {
  //blue light pixel moving around 
  uint8_t sinBeat = beatsin8(22, 0, NUM_LEDS - 1, 0, 0);
  leds[sinBeat] = CRGB::Blue;

  fadeToBlackBy(leds, NUM_LEDS, 10);
  EVERY_N_MILLISECONDS(10){
    Serial.println(sinBeat);
  }
  FastLED.show();

  
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////TEMP CODE ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
  //leds[0] = CRGB(r, g, b);
  //leds[0] = CRGB::Black; 
  //fill_solid(leds, NUM_LEDS, CRGB::Black);
  //fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue);
  //fill_rainbow(leds, NUM_LEDS, 0, 255/NUM_LEDS);
  //FastLED.show(); 
  //delay(500);

  //fill_gradient_RGB(leds, NUM_LEDS, CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue);
  //fill_rainbow(leds, NUM_LEDS, 0, 255/NUM_LEDS);
  //FastLED.show(); 
//  fill_solid(leds, NUM_LEDS, CRGB::Black);
//  FastLED.show(); 
//  delay(500);
//
