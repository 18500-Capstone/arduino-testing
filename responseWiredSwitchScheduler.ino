//Description: 

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

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// END OF LIGHT CODE DECLARATIONS ////////////////////////
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// MOTOR CODE DECLARATIONS ///////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int NUMWAVES = 5;
int NUMSYSM = 16;
int arrivingdatabyte;

const int motor13Pin = 3;
const int motor14Pin = 5;
const int motor7Pin = 6;
const int motor4Pin = 9;
const int motor2Pin = 10;
const int motor1Pin = 11;

////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// END OF MOTOR CODE DECLARATIONS ////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//Holds the current actions and (eventually) health level amount 
typedef struct {

  //FOR THE ACTUAL GAME DATA 
  int action; //one of 5 values: {FJ, HS, HL, LH, -}->{0, 1, 2, 3, 4}
  int healthLevel;  //holds number in range 0-25
} gameData;

gameData gameActions; //declaring instance of struct

//parses JSON request and sets the struct values accordingly 
void parse_request(String req, gameData *ga){

 String motorActionStr, lightActionStr = "";
 req = req.substring(1); //get rid of first character

 //setting motorAction and light substring
  motorActionStr = req; //1 hot vector "0010" FJ, HS, HL, LH
 
 //forceful jump state 1000
 if(motorActionStr.substring(0,1) == "1"){ //index corresponds to FJ
    //motorActionStr = "FJ";
    //ga->motorAction = motorActionStr;
    //ga->lightShowAction = motorActionStr;
    actionInt = 0; 
    ga->action = actionInt; 
    return; 
 }
 
 //Hit by a small enemy state 0100
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    //motorActionStr = "HS"; 
    //ga->motorAction = motorActionStr;
    //ga->lightShowAction = motorActionStr;
    actionInt = 1; 
    ga->action = actionInt; 
    return; 
 }
 
 //Hit by a large enemy state 0010
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    //motorActionStr = "HL";
    //ga->motorAction = motorActionStr;
    //ga->lightShowAction = motorActionStr;
    actionInt = 2; 
    ga->action = actionInt; 
    return; 
 }
 //low health state 0001
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    //motorActionStr = "LH";
    //ga->motorAction = motorActionStr;
    //ga->lightShowAction = motorActionStr;
    actionInt = 3; 
    ga->action = actionInt; 
    return; 
 }
 //default state (0000) where no relevant action is occuring
 if(motorActionStr.substring(0,1) == "0"){
    //motorActionStr = "-";
    //ga->motorAction = motorActionStr;
    //ga->lightShowAction = motorActionStr;
    actionInt = 4; 
    ga->action = actionInt; 
    return; 
 } 
}

void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
  
  //initializing pin modes for motors 
  pinMode(motor13Pin, OUTPUT);
  pinMode(motor14Pin, OUTPUT);
  pinMode(motor7Pin, OUTPUT);
  pinMode(motor4Pin, OUTPUT);
  pinMode(motor2Pin, OUTPUT);
  pinMode(motor1Pin, OUTPUT);
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
  
  //set each row of lights, w a delay of delayS in between 
  for(int i = 0; i < HEIGHT; i++){
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

void cascade(uint32_t colors[], int delayT, int delayS) {
  cascadeUp(colors, delayS);
  delay(delayT);
  cascadeDown(delayS);
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
void run_motors(uint8_t motors[], unsigned long powers[], unsigned long on_time) {
  unsigned long starttime = millis();
  unsigned long endtime = starttime;
  int MAGICNUM = 10;
  // run motors for specified time
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    for (int i=0; i < NUMSYSM; i++) {
      if (motors[i] != 0) {
        if (motors[i] == 1) {
          analogWrite(motor1Pin, powers[i]);
        }if (motors[i] == 2) {
          analogWrite(motor2Pin, powers[i]);
        }if (motors[i] == 4) {
          analogWrite(motor4Pin, powers[i]);
        }if (motors[i] == 7) {
          analogWrite(motor7Pin, powers[i]);
        }if (motors[i] == 13) {
          analogWrite(motor13Pin, powers[i]);
        }if (motors[i] == 14) {
          analogWrite(motor14Pin, powers[i]);
        }
      }
    }
    #ifdef ESP8266 
      yield(); // take a breather, required for ESP8266
    #endif 
    endtime = millis();
  }
  
  // turn all motors off
  // Serial.println("stopping motors");
  for (int i=0; i < sizeof(motors); i++) {
    analogWrite(motor1Pin, 0);
    analogWrite(motor2Pin, 0);
    analogWrite(motor4Pin, 0);
    analogWrite(motor7Pin, 0);
    analogWrite(motor13Pin, 0);
    analogWrite(motor14Pin, 0);
  }
}

/* Run a full haptic response
 *  IN: wave_motors : 2D array of motor values, one row for each wave
 *  IN: wave_powers : 2D array of motor power values, one row for each wave
 *  IN: on_times    : time in miliseconds for how long to delay between waves
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_response(uint8_t wave_motors[][16], unsigned long wave_powers[][16], 
  unsigned long on_times[], unsigned long delay_times[]) {
 
  // loop through and run each wave
  for (int i=0; i < NUMWAVES; i++) {
    run_motors(wave_motors[i], wave_powers[i], on_times[i]);
    //Sleep(delay_times[i]);
  }
}

// Hopefully I will change later. Hardcoded stuff that will go into a class
void run_response1() {
  // Try to run full response
  uint8_t wave_motors[][16] = {{7,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                               {4,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                               {1,2,13,14, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                               {4,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                               {7,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}};
  unsigned long wave_powers[][16] = {{127,127,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {127,127,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {127,127,127,127, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {127,127,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
                                     {127,127,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500,500,750,500,500};
  unsigned long delay_times[] = {20, 20, 20, 20, 20};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}


///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////END OF MOTOR CODE FNS  //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// helper-function for easy to use non-blocking timing
boolean TimePeriodIsOver (unsigned long &StartTime, unsigned long TimePeriod) {
  unsigned long currentMillis  = millis();  
  if ( currentMillis - StartTime >= TimePeriod ){
    StartTime = currentMillis; // store timestamp when the new interval has started
    return true;               // more time than TimePeriod) has elapsed since last time if-condition was true
  } 
  else return false; // return value false because LESS time than TimePeriod has passed by
}


void eventHandler(){
  currState = gameActions.action; 
  healthLevel = gameActions.healthLevel;
  
  switch(currState){
    
    case forcefulJump:
      run_response1();
      cascade(colorArray, 750, 500); //750 and 500 come from "on_times" array under motor functions
      strip.show(); //not sure if i need this
      break;
      
    case hitSmall:
      gotHit(); //light response 
      break;
      
    case hitLarge:
       gotHit(); //light response 
      break;
      
    case lowHealth:
       medium_LH(healthLevel); //light response 
      break;
     
    default: 
      //only run lights. No motor response
      strip.fill(purpleColor, 0, NUMPIXELS);
      strip.show(); 
      break; 
  }
}

void loop() {
  //recieve JSON response through a port and save it under "req"
  /////JSON response should be type "1000,NUM"
  /////where everything before comma is one-hot vector and num is curr health level 
  //parse response
  parse_request(req, &gameActions);
  
  runTests(); 

  //switch statements 
  //eventHandler(); 
  
}

//test out the parallelism of the actions. Looking to see if the motor/light responses even
//register individually, and how long they last (under the milli time requirement)
void runTests(){
  int tempHealthLevel = 25; 
  for(int i = 0; i < 5; i++){
    Serial.print("PARELLISM TEST FOR ");
    if(i == 0){
      Serial.println("FORCEFUL JUMP: ");
    } 
    if(i == 1){
      Serial.println("HIT SMALL: ");
    }
    if(i == 2){
      Serial.println("HIT LARGE: ");
    }
    if(i == 3){
      Serial.println("LOW HEALTH: ");
    }
    if(i == 4){
      Serial.println("no action: ");
    }
    gameActions.action = i;
    gameActions.healthLevel = tempHealthLevel-i; 
    eventHandler();
    delay(5000)
  }
}
