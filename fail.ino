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

int arrivingdatabyte;

struct tmpMotor{
  unsigned long on_time;
  int motors[4][16];
  long powers[4][16];
};

//void setMArrayNeg1(int * arrayPtr) {
//  int fullArray[sizeof(*arrayPtr)][sizeof((*arrayPtr)[0])] = *arrayPtr;
//  for (int i = 0; i < sizeof(fullArray); i++) {
//    for (int j = 0; j < sizeof(fullArray[i]); j++) {
//      fullArray[i][j]  = -1;
//    }
//  }
//}
//
//void setPArrayNeg1(long * fullArray) {
//  for (int i = 0; i < sizeof(fullArray); i++) {
//    for (int j = 0; j < sizeof(fullArray[i]); j++) {
//      fullArray[i][j]  = -1;
//    }
//  }
//  return fullArray
//}

/* Run a batch of motors at some specified powers for some time
 *  IN: motors : an array of motors to run
 *  IN: powers : an array the same size of motors for the power of each motor
 *  IN: on_time : time to keep the motors on
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_motors(uint8_t motors[], unsigned long powers[], unsigned long on_time) {
  unsigned long starttime = millis();
  unsigned long endtime = starttime;
  // run motors for specified time
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    for (int i=0; i < sizeof(motors) - 1; i++) {
      if (motors[i] != -1) {
        pwm.setPWM(motors[i], 0, powers[i]); // run each motor at their given power
        Serial.print("motors, powers: ");
        Serial.print(motors[i]);
        Serial.print(" ");
        Serial.println(powers[i]);
      }
    }
    #ifdef ESP8266 
      yield(); // take a breather, required for ESP8266
    #endif 
    endtime = millis();
  }
  
  // turn all motors off
  Serial.println("stopping motors");
  for (int i=0; i < sizeof(motors) - 1; i++) {
    if (motors[i] != -1) {
      pwm.setPWM(motors[i], 0, 0);
    }
  }
}

void run_response(uint8_t wave_motors[][16], unsigned long wave_powers[][16], 
  unsigned long on_times[], unsigned long delay_times[]) {
 
  // loop through and run each wave
  Serial.print("size of wave_motors: ");
  Serial.println(sizeof(wave_motors));

  for (int i=0; i < sizeof(wave_motors); i++) {
    // Set up arrays
    
    void setPArrayNeg1(long * fullArray) {
    
    Serial.print("about to run wave ");
    Serial.println(i);
    Serial.print("wave_motors[i]: ");
    for (int m = 0; m < sizeof(wave_motors[i]); m++) {
      if (wave_motors[i][m] != 0) {
        Serial.print(wave_motors[i][m]);
        Serial.print(" ");
      }
    }
    Serial.println("");
    Serial.print("wave_powers[i]: ");
    for (int m = 0; m < sizeof(wave_powers[i]); m++) {
      if (wave_powers != 0) {
        Serial.print(wave_powers[i][m]);
        Serial.print(" ");
      }
    }
    Serial.println("");
    run_motors(wave_motors[i], wave_powers[i], on_times[i]);

    // delay for next wave
    Serial.print("delaying for ");
    Serial.print(delay_times[i]);
    Serial.println(" milli seconds");
    delay(delay_times[i]);
  }
}

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

  // initialize arrays
  //  int mArray1[4][16];
  //  long pArray1[4][16];
  //  // long * mArray1_ptr = &mArray;
  //  setMArrayNeg1(&mArray1);
  //  setPArrayNeg1(&pArray1);
}

void loop() {
  // Read from serial port over and over
  if (Serial.available() > 0) {
    arrivingdatabyte = Serial.read(); //It will read the incoming or arriving data byte
    Serial.print("data byte received: ");
    Serial.println(arrivingdatabyte);

    
    // DO STUFF WITH INCOMING DATA
    if (arrivingdatabyte == 97) { // 97 == "a"
      // Try to run full response
      int wave_motors[][16] = {{1, 4}, {2,4,5}, {3,5}};
      long wave_powers[][16] = {{4090,4090}, {4090,4090}, {4090,4090}};
      long on_times[] = {500, 500, 500};
      long delay_times[] = {20, 20, 20};
      
      run_response(wave_motors, wave_powers, on_times, delay_times);
    }
  }
}
