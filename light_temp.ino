//File description: Controls light actions for forceful jump (in the form of a cascade light function), and low health. 
//Missing functions: hit small, hit large functions
//Drawn current information: requires about 20 mA for 1 'on' pixels + 1 mA per 'off' pixel.

//Author: Amelia Lopez 

#include <Adafruit_DotStar.h>
#include <string.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
//#define DATAPIN    4  //DI (blue)
//#define CLOCKPIN   5  //CI (green)

#define DATAPIN    12  //DI (red) 
#define CLOCKPIN   13  //CI (orange)

const int HEIGHT = 6;  //number of rows in the entire light system
const int WIDTH = 10;  //number of cols in the entire light system

//RGB channel setting colors
uint32_t offColor = 0x000000;
uint32_t blueColor = 0x0000FF;
//uint32_t redColor = 0xFF0000;
//uint32_t greenColor = 0x00FF00;
//uint32_t orangeColor = 0xeb8634;

//
//uint32_t yellowColor = 0xd4b01e;
//uint32_t yellowGreenColor = 0x82d41e;
//uint32_t greenAquaColor = 0x2dc485;
//uint32_t aquaColor = 0x2da1c4;
//uint32_t violetColor = 0x302dc4;
//uint32_t purpleColor = 0xa453d4;
//uint32_t pinkColor = 0xd453c3;
//uint32_t pinkRedColor = 0xd4537e;

//GRB channel setting colors
uint32_t redColor = 0x00FF00;      // 'On' color (starts red)
uint32_t greenColor = 0xFF0000;      // 'On' color (starts red)
uint32_t orangeColor = 0x34eb34;

uint32_t yellowColor = 0xb0d41e;
uint32_t yellowGreenColor = 0xd4821e;
uint32_t greenAquaColor = 0xc42d85;
uint32_t aquaColor = 0xa12dc4;
uint32_t lightAquaColor = 0xed80e2;

uint32_t lightGreenColor = 0xe34340;
uint32_t violetColor = 0x2d30c4;
uint32_t purpleColor = 0x53a4d4;
uint32_t pinkColor = 0x53d4c3;
uint32_t pinkRedColor = 0x53d47e;



int row0Indices[] = {0, 11, 12, 23, 24, 35, 36, 47, 48, 59}; //bottom row indexes
int row1Indices[] = {1, 10, 13, 22, 25, 34, 37, 46, 49, 58};
int row2Indices[] = {2, 9, 14, 21, 26, 33, 38, 45, 50, 57};
int row3Indices[] =  {3, 8, 15, 20, 27, 32, 39, 44, 51, 56};
int row4Indices[] = {4, 7, 16, 19, 28, 31, 40, 43, 52, 55};
int row5Indices[] = {5, 6, 17, 18, 29, 30, 41, 42, 53, 54}; //top row indexes
//int rowIndices[HEIGHT][WIDTH] = {{row0Indices}, {row1Indices}, {row2Indices}, {row3Indices}, {row4Indices}, {row5Indices}};
int rowIndices[HEIGHT][WIDTH] = {{0, 11, 12, 23, 24, 35, 36, 47, 48, 59}, 
{1, 10, 13, 22, 25, 34, 37, 46, 49, 58}, 
{2, 9, 14, 21, 26, 33, 38, 45, 50, 57}, 
{3, 8, 15, 20, 27, 32, 39, 44, 51, 56}, 
{4, 7, 16, 19, 28, 31, 40, 43, 52, 55}, 
{5, 6, 17, 18, 29, 30, 41, 42, 53, 54}};

//timing constants
unsigned long previousMillis1 = 0;        // will store last time LED was updated
unsigned long previousMillis2 = 0;        // will store last time LED was updated
unsigned long previousMillis3 = 0;        // will store last time LED was updated
unsigned long previousMillis4 = 0;        // will store last time LED was updated
unsigned long previousMillis5 = 0;        // will store last time LED was updated
unsigned long previousMillis6 = 0;        // will store last time LED was updated
unsigned long previousMillis7 = 0;        // will store last time LED was updated
long delayT = 750;           // milliseconds of time between cascade up and cascade down
long delayS = 500;          // milliseconds of time between rows being turned on 
long delayGotAKill = 250;        //ms
long delayGotHit = 250;          //ms
long delayLowHealthBasic = 100;  //ms
bool ledState1 = LOW; //got a hit
bool ledState2 = LOW; //got a kill 
bool ledState3 = LOW; //low health


Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG); 
// Hardware SPI is a little faster, but must be wired to specific pins
// (Arduino Uno = pin 11 for data, 13 for clock, other boards are different).  //test this !!!!!!!!!!!!
//Adafruit_DotStar strip(NUMPIXELS, DOTSTAR_BRG);

void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
}

//set a row of LEDs to 'color' and turn them on once done
void setRow(uint32_t color, int row) {
  //set the LED's rgb channel to color. Avoids doing memcpy 5 different times 
  for (int i = 0; i < WIDTH; i++) {
    strip.setPixelColor(rowIndices[row][i], color);
  }
  strip.show();
}

//gradally turn rows of lights on (starting at the bottom)
void cascadeUp(uint32_t colors[], int delayS) {
  //begin w lights off
  strip.fill(offColor, 0, NUMPIXELS);
  strip.show();
  unsigned long currentMillis7 = millis();
  //set each row of lights, w a delay of delayS in between 
  for(int i = 0; i < HEIGHT; i++){
    setRow(colors[i], i); //'strip.show()' is inside this function
    //delay(delayS);
    if(currentMillis7 - previousMillis7 >= delayT){
        previousMillis7 = currentMillis7;
        continue; 
     }
  }
}

//gradually turn rows of lights off (starting at the top)
void cascadeDown(long delayS) {
   unsigned long currentMillis6 = millis();
   for(int i = (HEIGHT-1); i > -1; i--){
    setRow(offColor, i); //'strip.show()' is inside this function
    //delay(delayS);
     if(currentMillis6 - previousMillis6 >= delayT){
        previousMillis6 = currentMillis6;
        continue; 
     }
  }
}

//cascades lights
//colors[] is an array of RGB color values, color[0] is color for 0th row. color[1] is color for 1st row, and so forth 
//DelayT is the time between the end of cascade up and the start of cascade down
//DelayS is the time between a row turning on.
void cascade(uint32_t colors[], long delayT, long delayS) {
  cascadeUp(colors, delayS);
  unsigned long currentMillis5 = millis();
  if(currentMillis5 - previousMillis5 >= delayT){
     //delay(delayT);
     cascadeDown(delayS);
     previousMillis5 = currentMillis5;
  }
}

//basic low health pattern, flashes red
void basic_LH() {
  unsigned long currentMillis4 = millis();
    if((ledState4 == HIGH) && (currentMillis4 - previousMillis4 >= delayLowHealthBasic)){
       ledState4 = LOW;
       previousMillis4 = currentMillis4;
       strip.fill(offColor, 0, NUMPIXELS);
       strip.show(); 
    }
    else if((ledState4 == LOW) && (currentMillis4 - previousMillis4 >= delayLowHealthBasic)){
       ledState4 = HIGH;
       previousMillis4 = currentMillis4;
       strip.fill(redColor, 0, NUMPIXELS);
       strip.show(); 
    }  
  
//  strip.fill(redColor, 0, NUMPIXELS);
//  strip.show();
//  delay(250);
//  strip.fill(offColor, 0, NUMPIXELS);
//  strip.show();
//  delay(150);
}

//medium low health patter, frequenct of red flashes increase as healthlevel gets smaller
void medium_LH(int healthLevel) {
  unsigned long delayLowHealth = (unsigned long) (unsigned int) healthLevel ;
  //say low health is at a range 0-25
  unsigned long currentMillis3 = millis();
    if((ledState3 == HIGH) && (currentMillis3 - previousMillis3 >= delayLowHealth)){
       ledState3 = LOW;
       previousMillis3 = currentMillis3;
       strip.fill(offColor, 0, NUMPIXELS);
       strip.show(); 
    }
    else if((ledState3 == LOW) && (currentMillis3 - previousMillis3 >= delayLowHealth)){
       ledState3 = HIGH;
       previousMillis3 = currentMillis3;
       strip.fill(redColor, 0, NUMPIXELS);
       strip.show(); 
    }  
//  strip.fill(redColor, 0, NUMPIXELS);
//  strip.show();
//  delay(healthLevel);
//  strip.fill(offColor, 0, NUMPIXELS);
//  strip.show();
//  delay(healthLevel);
}

void gotHitLoop(){
    unsigned long currentMillis1 = millis();
    if((ledState1 == HIGH) && (currentMillis1 - previousMillis1 >= delayGotHit)){
       ledState1 = LOW;
       previousMillis1 = currentMillis1;
       strip.fill(offColor, 0, NUMPIXELS);
       strip.show(); 
    }
    else if((ledState1 == LOW) && (currentMillis1 - previousMillis1 >= delayGotHit)){
       ledState1 = HIGH;
       previousMillis1 = currentMillis1;
       strip.fill(redColor, 0, NUMPIXELS);
       strip.show(); 
    }  
}

void gotHit(){
  gotHitLoop();
  gotHitLoop(); 
}

void gotAKillLoop(){
    unsigned long currentMillis2 = millis();
    if((ledState2 == HIGH) && (currentMillis2 - previousMillis2 >= delayGotHit)){
       ledState2 = LOW;
       previousMillis2 = currentMillis2;
       strip.fill(offColor, 0, NUMPIXELS);
       strip.show(); 
    }
    else if((ledState2 == LOW) && (currentMillis2 - previousMillis2 >= delayGotHit)){
       ledState2 = HIGH;
       previousMillis2 = currentMillis2;
       strip.fill(greenColor, 0, NUMPIXELS);
       strip.show(); 
    }     
}

void gotAKill(){
  gotAKillLoop();
  gotAKillLoop(); 
}

void defaultLights(){
  strip.fill(purpleColor, 0, NUMPIXELS);
  strip.show(); 
}


void loop() {
  //color at index0 will appear in row0, index1 will appear in row1, ... and so forth 
  uint32_t colorArray[] = {pinkColor, violetColor, aquaColor, yellowGreenColor, orangeColor, redColor};  
  //uint32_t colorArray[] = {lightAquaColor, lightAquaColor, greenAquaColor, greenAquaColor, aquaColor, aquaColor};  
  //uint32_t colorArray[] = {lightGreenColor, lightGreenColor, greenAquaColor, greenAquaColor, greenColor, greenColor};  
  cascade(colorArray, 100, 75); 
  strip.show(); 
  
 

  
}
