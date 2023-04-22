/*Description: This file takes in data from the game "The Last Spartan" from nodeJS 
 * through a serial COM port (wired) and outputs a series of motor and light responses
 * for a unique in-game action. In order to multi-task responses (i.e. light responses 
 * and motor response at the "same" time), the algorithms use millis() for timing instead 
 * of delay(), and switch statements to improve efficiency. 
 * 
 * It responds to the following 4 in-game actions:
 *      1) Forceful jump
 *      2) Hit by small enemy
 *      3) Hit by large enemy
 *      4) Low health 
 *     
 * 
 * Ways to improve: define a motorResponse and lightResponse function class in order to simplify the code and 
 * make it easier to replicate. motorResponse and lightResponse would run motor/light responses within a specified time.
 * 
 */

//Author: Amelia Lopez

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////LIGHT CODE DECLARATIONS /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#include <Adafruit_DotStar.h>
#include <string.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    12  //DI (blue)
#define CLOCKPIN   13  //CI (green)

const int HEIGHT = 6;  //number of rows in the entire light system
const int WIDTH = 10;  //number of cols in the entire light system

//GRB channel setting colors
uint32_t offColor = 0x000000;
uint32_t blueColor = 0x0000FF;
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
uint32_t colorArray[] = {pinkColor, violetColor, aquaColor, yellowGreenColor, orangeColor, redColor};  


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
unsigned long previousMillis8 = 0;        // will store last time LED was updated
long delayT = 750;           // milliseconds of time between cascade up and cascade down
long delayS = 500;          // milliseconds of time between rows being turned on 
long delayGotAKill = 200;        //ms
long delayGotHit = 200;          //ms
long delayLowHealthBasic = 100;  //ms
bool ledState1 = LOW; //got a hit
bool ledState2 = LOW; //got a kill 
bool ledState3 = LOW; //low health
bool ledState4 = LOW; //low health temp for basic

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// END OF LIGHT CODE DECLARATIONS ////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// MOTOR CODE DECLARATIONS ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int NUMWAVES = 5;
int NUMSYSM = 16;
int arrivingdatabyte;


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// END OF MOTOR CODE DECLARATIONS ////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//buffer declaration for reading in bytes
const int BUFFER_SIZE = 1000;
char buf[BUFFER_SIZE];

const int forcefulJump = 0;
const int hitSmall = 1; 
const int hitLarge = 2;
const int lowHealth = 3; 

//Holds the current action, health level amount, and desired motor intensity 
typedef struct {
  int action; //one of 5 int values: {FJ, HS, HL, LH, -}->{0, 1, 2, 3, 4}
  int desiredMotorIntensity; //holds number in range 0-100
  int healthLevel;          //holds number in range 0-100
} gameData;

gameData gameActions; //declaring instance of struct 

//parses JSON request and sets the struct values accordingly 
void parse_request(String req, gameData *ga){

 //declarations
 char delimitC = ',';
 String motorActionStr, healthNumStr, motorStrengthStr = "";
 req = req.substring(1); //get rid of first character
 
 //getting index 
 int indexOf1stComma = req.indexOf(delimitC);
 int indexOfLastComma = req.lastIndexOf(delimitC); 
 int indexOfLastChar = req.length(); 
 
 //isolating motorAction, health num, and motor strength substring
 motorActionStr = req.substring(0,indexOf1stComma); //1 hot vector "0010" FJ, HS, HL, LH
 healthNumStr = req.substring(indexOf1stComma+1, indexOfLastComma);
 motorStrengthStr = req.substring(indexOfLastComma+1);
  
 //converting healthNumStr and motorStrengthStr to int and setting the struct values 
 if(healthNumStr == "NaN"){
  ga->healthLevel = 100;
 }
 else{
  ga->healthLevel = healthNumStr.toInt();
 }
 ga->desiredMotorIntensity = motorStrengthStr.toInt(); 
 
  
 //forceful jump state 1000
 if(motorActionStr.substring(0,1) == "1"){ //index corresponds to FJ
    motorActionStr = "FJ";
    ga->action = 0;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 
 ////Hit by a small enemy state 0100
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HS"; 
    ga->action = 1;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 
 //Hit by a large enemy state 0010
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HL";
    ga->action = 2;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 //low health state 0001
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "LH";
    ga->action = 3;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 //default state (0000) where no relevant action is occuring
 if(motorActionStr.substring(0,1) == "0"){
    motorActionStr = "-";
    ga->action = 4;
    //ga->lightShowAction = motorActionStr;
    return; 
 } 
}

void setup() {
  strip.begin(); // Initialize pins for output
  //strip.fill(blueColor, 0, NUMPIXELS);
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);

  //motor setup
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);
  
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////LIGHT CODE FNS //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


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
}

void gotHitLargeLoop(){
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
       strip.fill(blueColor, 0, NUMPIXELS);
       strip.show(); 
    }  
}

void gotHitSmallLoop(){
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
       strip.fill(yellowGreenColor, 0, NUMPIXELS);
       strip.show(); 
    }  
}

void gotHitSmall(){
  gotHitSmallLoop();
  gotHitSmallLoop(); 
}

void gotHitLarge(){
  gotHitLargeLoop();
  gotHitLargeLoop(); 
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
  strip.fill(violetColor, 0, NUMPIXELS);
  strip.show(); 
}

///////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// END OF LIGHT CODE FNS //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////MOTOR CODE FNS  /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

/* Run a batch of motors at some specified powers for some time
 *  IN: motors : an array of motors to run
 *  IN: powers : an array the same size of motors for the power of each motor
 *  IN: on_time : time to keep the motors on
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_motors(int motors[], unsigned long powers[], unsigned long on_time, int num_on) {
  unsigned long starttime = millis();
  unsigned long endtime = starttime;
  int MAGICNUM = 10;
  // PRINT OUT MOTORS

  Serial.print("size of wave ");
  Serial.print(num_on);
  Serial.println("");


  
  Serial.print("\nwave_motors[i]: ");
    for (int m = 0; m < NUMSYSM; m++) {
      if (motors[m] != -1) {
        Serial.print(motors[m]);
        Serial.print(" ");
      }
    }
    Serial.println("");
    Serial.print("wave_powers[i]: ");
    for (int m = 0; m < NUMSYSM; m++) {
      if (powers[m] != 0) {
        Serial.print(powers[m]);
        Serial.print(" ");
      }
    }
    Serial.println("");
  
  // run motors for specified time
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    // if only one motor has to turn on, turn it on to full power
    if (num_on < 2) {
      for (int i=0; i < NUMSYSM; i++) {
        if (motors[i] != -1) {
          pwm.setPWM(motors[i], 0, powers[i]); // run each motor at their given power
//          pwm.writeMicroseconds(motors[i], MAGICNUM);
        }
      }
      #ifdef ESP8266 
        yield(); // take a breather, required for ESP8266
      #endif 
      endtime = millis();
    }

    else { // if there are 2+ motors that are supposed to be on at the same time
      for (uint16_t i=0; i<4096; i += 8) {
        for (uint8_t pwmidx=0; pwmidx < 16; pwmidx++) {
          if (motors[pwmidx] != -1) {
            pwm.setPWM(motors[pwmidx], 0, (i + (4096/num_on)*motors[pwmidx]) % powers[pwmidx] );
//            pwm.setPWM(motors[pwmidx], 0, (powers[pwmidx] + (4096/num_on)*motors[pwmidx]) % powers[pwmidx] );
          }
        }
          
        }
      #ifdef ESP8266 
        yield(); // take a breather, required for ESP8266
      #endif 
      endtime = millis();
    }
  }
  
  // turn all motors off
  // Serial.println("stopping motors");
  for (int i=0; i < NUMSYSM; i++) {
    pwm.setPWM(i, 0, 0);
  }
}

/* Run a full haptic response
 *  IN: wave_motors : 2D array of motor values, one row for each wave
 *  IN: wave_powers : 2D array of motor power values, one row for each wave
 *  IN: on_times    : time in miliseconds for how long to delay between waves
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_response(int wave_motors[][16], unsigned long wave_powers[][16], 
  unsigned long on_times[], unsigned long delay_times[]) {
 
  // loop through and run each wave
  Serial.print("size of wave_motors: ");
  Serial.println(sizeof(wave_motors));
  int num_on; 
  for (int i=0; i < NUMWAVES; i++) {
    Serial.print("\nabout to run wave ");
    Serial.println(i);
    
    // calculate how many motors to turn on / have to be cycled through
    num_on = 0;
    for (int m=0; m < NUMSYSM; m++) {
      if (wave_motors[i][m] != -1) {
        num_on++;
      }
    }

    if (num_on > 0) {
      run_motors(wave_motors[i], wave_powers[i], on_times[i], num_on);
    }

    // delay for next wave
    Serial.print("delaying for ");
    Serial.print(delay_times[i]);
    Serial.println(" milli seconds");
    delay(delay_times[i]);
  }
}

// Hopefully I will change later. Hardcoded stuff that will go into a class
void run_response1(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{2,7,3,6, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {1,5,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {0,4,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {1,5,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,7,3,6, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4096*i,4096*i,4096*i,4096*i, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
                                     {4090*i,4090*i,4090*i,4090*i, 0,0,0,0, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500,500,750,500,500};
  unsigned long delay_times[] = {20, 20, 20, 20, 20};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}

// Responce 2 is the hit by small enemy
void run_response2(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{3,6,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,7,1,5, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,0*i,0*i,0*i,             0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,0*i,       0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500, 750, 0, 0, 0, 0};
  unsigned long delay_times[] = {20, 0, 0, 0, 0};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}

// Responce 3 is the hit by large enemy
void run_response3(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{3,6,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,7,1,5, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {0,4,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,0*i,0*i,0*i,             0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,0*i,       0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500, 750, 250, 0, 0, 0};
  unsigned long delay_times[] = {20, 20, 0, 0, 0};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}

// Responce 4 is the low health
void run_response4(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{4,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {4,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {4,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {4,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,0*i,0*i,0*i,             0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,0*i,       0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {200, 200, 200, 200, 200, 0};
  unsigned long delay_times[] = {20, 20, 20, 20, 200};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}



///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////END OF MOTOR CODE FNS  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////



void eventHandler(){
  int currState = gameActions.action; 
  int healthLevel = gameActions.healthLevel;
  int motorIntensity = gameActions.desiredMotorIntensity;
  
  switch(currState){
    
    case (forcefulJump):
      run_response1(motorIntensity); //motor response 
      strip.fill(greenColor, 0, NUMPIXELS);
      strip.show(); //not sure if i need this
      break;
      
    case (hitSmall):
      run_response2(motorIntensity); //motor response
      strip.fill(lightGreenColor, 0, NUMPIXELS);
      strip.show();
      break;
      
    case (hitLarge):
       run_response3(motorIntensity);
       strip.fill(blueColor, 0, NUMPIXELS);
       strip.show();
       gotHitLarge();  
      break;
      
    case (lowHealth):
       run_response4(motorIntensity);
       strip.fill(redColor, 0, NUMPIXELS);
       strip.show(); 
      break;
     
    default: 
      //only run lights. No motor response
      defaultLights(); 
      break; 
  }
}

void loop() {
  int startingIndex = 0; 

  // check if data is available
  if(Serial.available() > 0) {

    while(Serial.available() > 0)
    {
      //read the incoming string:
      int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
      String incomingReq = "";
      for(int i = startingIndex; i < rlen+startingIndex; i++){
        incomingReq += char(buf[i]);    //eventually clear buffer 
        startingIndex = rlen;  
      }
      parse_request(incomingReq, &gameActions);
      eventHandler();  
    }
  }
}


void runTests(){
    Serial.println("run testssssss");
    bool doFJ = true;
    bool doHS = false;
    bool doHL = false;
    bool doLH = false; 
    unsigned long currentMillis8 = millis();
    if(currentMillis8 - previousMillis8 >= 5000){
      if(doFJ){
        //FJ
        Serial.println("doing FJ response");
        run_response1(80); //motor response
        //cascade(colorArray, 750, 500); //750 and 500 come from "on_times" array under motor functions
        strip.fill(blueColor, 0, NUMPIXELS);
        doFJ = false;
        doHS = true; 
      }
      if(doHS){
        //hit small 
        Serial.println("doing HS response");
        run_response2(80); //motor response
        
        doHS = false;
        doHL = true; 
      }
      if(doHL){
        //hit large 
        Serial.println("doing HL response");
        run_response3(80);
        
        doHL = false;
        doLH = true;
      }
      if(doLH){
        //low health
        Serial.println("doing Low health response");
        run_response4(80);
        
        doLH = false;
        doFJ = true; 
      }
      previousMillis8 = currentMillis8;
    }
}
