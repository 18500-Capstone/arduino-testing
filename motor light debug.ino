int NUMWAVES = 5;
int NUMSYSM = 16;
int arrivingdatabyte;

const int motor13Pin = 3;
const int motor14Pin = 5;
const int motor7Pin = 6;
const int motor4Pin = 9;
const int motor2Pin = 10;
const int motor1Pin = 11;

#include <Adafruit_DotStar.h>
#include <string.h>
// Because conditional #includes don't work w/Arduino sketches...
#include <SPI.h>         // COMMENT OUT THIS LINE FOR GEMMA OR TRINKET
//#include <avr/power.h> // ENABLE THIS LINE FOR GEMMA OR TRINKET

#define NUMPIXELS 60 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13  //DI (blue)
#define CLOCKPIN   12  //CI (green)

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG); //might 

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel PWM test!");

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP

}

//RGB channel setting colors
uint32_t offColor = 0x000000;
uint32_t blueColor = 0x0000FF;
uint32_t redColor = 0x00FF00;      // 'On' color (starts red)
uint32_t greenColor = 0xFF0000;      // 'On' color (starts red)
uint32_t orangeColor = 0x34eb34;
uint32_t purpleColor = 0x53a4d4;
uint32_t pinkColor = 0x53d4c3;


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
          strip.fill(redColor, 0, NUMPIXELS);
          strip.show();
        }if (motors[i] == 2) {
          analogWrite(motor2Pin, powers[i]);
           strip.fill(blueColor, 0, NUMPIXELS);
          strip.show();
        }if (motors[i] == 4) {
          analogWrite(motor4Pin, powers[i]);
           strip.fill(greenColor, 0, NUMPIXELS);
          strip.show();
        }if (motors[i] == 7) {
          analogWrite(motor7Pin, powers[i]);
           strip.fill(pinkColor, 0, NUMPIXELS);
          strip.show();
        }if (motors[i] == 13) {
          analogWrite(motor13Pin, powers[i]);
           strip.fill(orangeColor, 0, NUMPIXELS);
          strip.show();
        }if (motors[i] == 14) {
          analogWrite(motor14Pin, powers[i]);
           strip.fill(purpleColor, 0, NUMPIXELS);
          strip.show();
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
