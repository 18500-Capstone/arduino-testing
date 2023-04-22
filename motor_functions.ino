/*************************************************** 
  This is an example for our Adafruit 16-channel PWM & Servo driver
  PWM test - this will drive 16 PWMs in a 'wave'

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These drivers use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int NUMWAVES = 5;
int NUMSYSM = 16;
int arrivingdatabyte;


void setup() {
  Serial.begin(9600);
  Serial.println("16 channel PWM test!");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);
}

//struct tmpMotor{
//  unsigned long on_time;
//  int motors[4][16];
//  long powers[4][16];
//};


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

void loop() {
  // Read from serial port over and over
  if (Serial.available() > 0) {
    arrivingdatabyte = Serial.read(); //It will read the incoming or arriving data byte
    Serial.print("data byte received: ");
    Serial.println(arrivingdatabyte);


    // DO STUFF WITH INCOMING DATA
    if (arrivingdatabyte == 97) { // 97 == "a"
      run_response1(100);
    }
  }
}
