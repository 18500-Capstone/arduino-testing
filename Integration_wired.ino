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

const int motor16pin1 = 5;
const int motor16pin2 = 6;
const int motor17pin1 = 9;
const int motor17pin2 = 10;


////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// END OF MOTOR CODE DECLARATIONS ////////////////////////
////////////////////////////////////////////////////////////////////////////////////

//buffer declaration for reading in bytes
const int BUFFER_SIZE = 600;
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
  Serial.begin(9600);
  Serial.println("Begin");
  //lightcode set up 
  FastLED.addLeds<DOTSTAR, LED_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(100);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show(); 

  //Fill the colorIndex array with random numbers
  for (int i = 0; i < NUM_LEDS; i++) {
    colorIndex[i] = random8();
  }

  //motor setup
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);

  pinMode(motor16pin1, OUTPUT);
  pinMode(motor16pin2, OUTPUT);
  pinMode(motor17pin1, OUTPUT);
  pinMode(motor17pin2, OUTPUT);
  
}

///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////LIGHT CODE FNS //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


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
  EVERY_N_MILLISECONDS(100){
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

void defaultLights(){
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
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
  
  // run motors for specified time
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    // if only one motor has to turn on, turn it on to full power
    if (num_on < 2) {
      for (int i=0; i < NUMSYSM; i++) {
        if (motors[i] != -1) {
          if (motors[i] != 16 && motors[i] != 17) {
            pwm.setPWM(motors[i], 0, powers[i]); // run each motor at their given power
          }
          else {
            if (motors[i] == 16) {
              // turn on motor system 16
              analogWrite(motor16pin1, powers[i]);
              analogWrite(motor16pin2, powers[i]);
            }
            if (motors[i] == 17){
              // turn on motor system 17
              analogWrite(motor17pin1, powers[i]);
              analogWrite(motor17pin2, powers[i]);
            }
          }
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
            if (motors[pwmidx] != 16 && motors[pwmidx] != 17) {
              pwm.setPWM(motors[pwmidx], 0, (i + (4096/num_on)*motors[pwmidx]) % powers[pwmidx] );
            }
            else {
            if (motors[i] == 16) {
              // turn on motor system 16
              analogWrite(motor16pin1, powers[i]);
              analogWrite(motor16pin2, powers[i]);
            }
            if (motors[i] == 17){
              // turn on motor system 17
              analogWrite(motor17pin1, powers[i]);
              analogWrite(motor17pin2, powers[i]);
            }
          }
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
  analogWrite(motor16pin1, 0);
  analogWrite(motor16pin2, 0);
  analogWrite(motor17pin1, 0);
  analogWrite(motor17pin2, 0);
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
  int num_on; 
  for (int i=0; i < NUMWAVES; i++) {    
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
    delay(delay_times[i]);
  }
}

// Hopefully I will change later. Hardcoded stuff that will go into a class
void run_response1(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{6,7,3,o, 15,17,13,16, o,o,o,o, o,o,o,o}, 
                           {2,1,5,10, 11,o,o,o, o,o,o,o, o,o,o,o}, 
                           {0,4,8,9, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,1,5,10, 11,16,o,o, o,o,o,o, o,o,o,o}, 
                           {6,7,3,o, 15,17,13,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4096*i,4096*i,4096*i,4096*i, 4096*i,150*i,4096*i,150*i, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,4096*i,4096*i, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,4096*i,4096*i, 0,0,0,0, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,4096*i,4096*i, 4096*i,250*i,0,0, 0,0,0,0, 0,0,0,0},
                                     {4090*i,4090*i,4090*i,4090*i, 4096*i,250*i,4096*i,4096*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500,500,750,500,500};
  unsigned long delay_times[] = {20, 20, 20, 20, 20};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}

// Responce 2 is the hit by small enemy
void run_response2(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{3,5,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,7,1,6, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,0*i,0*i,0*i,             0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,0*i,       0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500, 750, 0, 0, 0};
  unsigned long delay_times[] = {20, 0, 0, 0, 0};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}

// Responce 3 is the hit by large enemy
void run_response3(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int wave_motors[][16] = {{0,4,1,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {2,5,7,3, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {12,14,13,15, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,4090*i,0*i,0*i,          0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,4090*i,    4090*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4096*i,4096*i,4096*i,4096*i,    4090*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {500, 500, 250, 0, 0};
  unsigned long delay_times[] = {20, 20, 20, 0, 0};
  
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
                                     {4090*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {200, 200, 200, 200, 200, 0};
  unsigned long delay_times[] = {40, 40, 40, 40, 0};
  
  run_response(wave_motors, wave_powers, on_times, delay_times);
}


// Responce 5 is for debugging
void run_response5(int val) {
  int i = val / 100;
  // Try to run full response
  int o = -1;
  int m = 0;
  int wave_motors[][16] = {{m,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {m,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {m,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}, 
                           {o,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o}};
  unsigned long wave_powers[][16] = {{4090*i,0*i,0*i,0*i,             0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,4090*i,4090*i,0*i,       0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,255*i,4090*i,4090*i,    0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {4090*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}, 
                                     {0*i,0*i,0*i,0*i,                0*i,0*i,0*i,0*i, 0,0,0,0, 0,0,0,0}};
  unsigned long on_times[] = {1000, 1000, 2000, 2000, 0};
  unsigned long delay_times[] = {500, 500, 500, 200, 0};
  
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
    case (forcefulJump): //case 0
      run_response1(100); //motor response 
      FJ_LightShow();
      break;
    case (hitSmall):  //case 1
      run_response2(100); //motor response
      hitSmall_LightShow(); 
      break;
      
    case (hitLarge):  //case 2
      run_response3(100);
      hitLarge_LightShow();
      break;
      
    case (lowHealth): //case 3
      run_response4(100);
      lowHealth_LightShow();
      break;
     
//    default: 
//      //only run lights. No motor response
//      defaultLights(); 
//      break; 
  }
}

//void loop() {
//  int startingIndex = 0; 
//  // check if data is available
//  if(Serial.available() > 0) {
//    fill_solid(leds, NUM_LEDS, CRGB::Blue);
//    FastLED.show();
//    while(Serial.available() > 0)
//    {
//      //read the incoming string:
//      int rlen = Serial.readBytesUntil('\n', buf, BUFFER_SIZE);
//      String incomingReq = "";
//      for(int i = startingIndex; i < rlen+startingIndex; i++){
//        incomingReq += char(buf[i]);    //eventually clear buffer 
//        startingIndex = rlen;  
//      }
//      parse_request(incomingReq, &gameActions);
//      eventHandler(); 
//      
//      //clear buffer BEFORE it gets too full
//      if(startingIndex + 100 >= BUFFER_SIZE) {
//        memset(buf, 0, sizeof(buf)); //buf[0] = char(0)
//        startingIndex = 0;          //update starting index
//      }
//       
//    }
//  }
//}
long tmpTime = 0;
void loop(){
    tmpTime = millis();
    Serial.println(patternCounter);
    // This is jump
    if(patternCounter == 0){
      Serial.println("FJ");
      run_response1(80);
      tmpTime = millis();
      while (millis() - tmpTime < 1000) {
        FJ_LightShow();
      }      
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();  
    }
    // This is small hit
    else if(patternCounter == 1){
      Serial.println("small hit");
      run_response2(80); //motor response 
      tmpTime = millis();
      while (millis() - tmpTime < 1000) {
        hitSmall_LightShow();
      }
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
      
    }
    // This is large hit
    else if(patternCounter == 2){
      Serial.println("large hit");
      run_response3(80); //motor response 
      tmpTime = millis();
      while (millis() - tmpTime < 1000) {
        hitLarge_LightShow();
      } 
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }
    // This is low health
    else if(patternCounter == 3){
      Serial.println("Low Health");
      run_response1(80); //motor response
      tmpTime = millis();
      while (millis() - tmpTime < 1000) {
        lowHealth_LightShow();
      }      
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }
  EVERY_N_SECONDS(5){
    nextPattern();
  }
  //fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.show();
}

void nextPattern(){
  patternCounter = (patternCounter + 1) % 4;
}
