//File description: Controls light actions for forceful jump (in the form of a cascade light function), and low health. 
//Missing functions: hit small, hit large functions
//Drawn current information: requires about 200 mA for all the 'on' pixels + 1 mA per 'off' pixel.

//Author: Amelia Lopez 

#include <Adafruit_DotStar.h>
#include <string.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    4  //DI
#define CLOCKPIN   5  //CI

const int HEIGHT = 6;  //number of rows in the entire light system
const int WIDTH = 10;  //number of cols in the entire light system

//RGB channel setting colors
uint32_t offColor = 0x000000;
uint32_t blueColor = 0x0000FF;
uint32_t redColor = 0xFF0000;
uint32_t greenColor = 0x00FF00;
uint32_t orangeColor = 0xd47c1e;

uint32_t yellowColor = 0xd4b01e;
uint32_t yellowGreenColor = 0x82d41e;
uint32_t greenAquaColor = 0x2dc485;
uint32_t aquaColor = 0x2da1c4;
uint32_t violetColor = 0x302dc4;
uint32_t purpleColor = 0xa453d4;
uint32_t pinkColor = 0xd453c3;
uint32_t pinkRedColor = 0xd4537e;

//GRB channel setting colors
//uint32_t redColor = 0x00FF00;      // 'On' color (starts red)
//uint32_t greenColor = 0xFF0000;      // 'On' color (starts red)

const int row0Indices[] = {0, 11, 12, 23, 24, 35, 36, 47, 48, 59}; //bottom row indexes
const int row1Indices[] = {1, 10, 13, 22, 25, 34, 37, 46, 49, 58};
const int row2Indices[] = {2, 9, 14, 21, 26, 33, 38, 45, 50, 57};
const int row3Indices[] =  {3, 8, 15, 20, 27, 32, 39, 44, 51, 56};
const int row4Indices[] = {4, 7, 16, 19, 28, 31, 40, 43, 52, 55};
const int row5Indices[] = {5, 6, 17, 18, 29, 30, 41, 42, 53, 54}; //top row indexes


Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_RGB); //might need to change to BRG or remove 4th parameter
// The last parameter is optional -- this is the color data order of the
// DotStar strip, which has changed over time in different production runs.
// Your code just uses R,G,B colors, the library then reassigns as needed.
// Default is DOTSTAR_BRG, so change this if you have an earlier strip.

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
  int rowIndx[WIDTH];
  //set the correct array to index into
  if (row == 0)  memcpy(rowIndx, row0Indices, sizeof(row0Indices));  //memcpy size field is in bytes
  else if (row == 1) memcpy(rowIndx, row1Indices, sizeof(row1Indices));
  else if (row == 2) memcpy(rowIndx, row2Indices, sizeof(row2Indices));
  else if (row == 3)  memcpy(rowIndx, row3Indices, sizeof(row3Indices));
  else if (row == 4)  memcpy(rowIndx, row4Indices, sizeof(row4Indices));
  else  memcpy(rowIndx, row5Indices, sizeof(row5Indices));
  //Serial.println("rowwww: " + String(row));

//  //test that rowIndx is the correct array (i.e. working correctly)
//  for (int i = 0; i < WIDTH; i++) {
//    //Serial.println("here" + String(row));
//    if (row == 0) { 
//      Serial.print(rowIndx[i] == row0Indices[i]);
//    }
//    else if (row == 1) Serial.print(rowIndx[i] == row1Indices[i]);
//    else if (row == 2) Serial.print(rowIndx[i] == row2Indices[i]);
//    else if (row == 3) Serial.print(rowIndx[i] == row3Indices[i]);
//    else if (row == 4) Serial.print(rowIndx[i] == row4Indices[i]);
//    else Serial.print(rowIndx[i] == row5Indices[i]);
//  }
//  Serial.println();

  //set the row to the pixel color
  for (int i = 0; i < WIDTH; i++) {
    strip.setPixelColor(rowIndx[i], color);
  }
  strip.show();
}

//gradally turn rows of lights on (starting at the bottom)
void cascadeUp(uint32_t colors[], int delayS) {
  //begin w lights off
  strip.fill(offColor, 0, NUMPIXELS);
  strip.show();
  
  //set each row of lights, w a delay of delayS in between 
  for(int i = 0; i < HEIGHT; i++){
    Serial.println(String(i));
    Serial.println(String(colors[i], HEX));
    setRow(colors[i], i); //'strip.show()' is inside this function
    delay(delayS);
  }
}

//gradually turn rows of lights off (starting at the top)
void cascadeDown(int delayS) {
   for(int i = (HEIGHT-1); i > -1; i--){
    setRow(offColor, i); //'strip.show()' is inside this function
    delay(delayS);
  }
}

//cascades lights
//colors[] is an array of RGB color values, color[0] is color for 0th row. color[1] is color for 1st row, and so forth 
//DelayT is the time between the end of cascade up and the start of cascade down
//DelayS is the time between a row turning on.
void cascade(uint32_t colors[], int delayT, int delayS) {
  cascadeUp(colors, delayS);
  delay(delayT);
  cascadeDown(delayS);
}

void basic_LH() {
  strip.fill(redColor, 0, NUMPIXELS);
  strip.show();
  delay(250);
  strip.fill(offColor, 0, NUMPIXELS);
  strip.show();
  delay(150);
}

void medium_LH(int healthLevel) {
  //say low health is at a range 0-25
  strip.fill(redColor, 0, NUMPIXELS);
  strip.show();
  delay(250);
  strip.fill(offColor, 0, NUMPIXELS);
  strip.show();
  delay(healthLevel);
}

void loop() {
  //color at index0 will appear in row0, index1 will appear in row1, ... and so forth 
  uint32_t colorArray[] = {pinkColor, violetColor, aquaColor, yellowGreenColor, orangeColor, redColor};  
  cascade(colorArray, 200, 150);
  //  strip.fill(blueColor, 0, 30);

}
