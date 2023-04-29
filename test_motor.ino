#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int NUMWAVES = 1;
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

/* Run a batch of motors at some specified powers for some time
 *  IN: motors : an array of motors to run
 *  IN: powers : an array the same size of motors for the power of each motor
 *  IN: on_time : time to keep the motors on
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_motors(int motors[], unsigned long powers[], unsigned long on_time, int num_on) {
  unsigned long starttime = millis();
  unsigned long endtime = starttime;

  // run motors for specified time
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    for (int i=0; i < NUMSYSM; i++) {
      if (motors[i] != -1) {
        pwm.setPWM(motors[i], 0, powers[i]); // run each motor at their given power
      }
    }
    #ifdef ESP8266 
      yield(); // take a breather, required for ESP8266
    #endif 
    endtime = millis();
  }

  // turn all motors off
  // Serial.println("stopping motors");
  for (int i=0; i < NUMSYSM; i++) {
    pwm.setPWM(i, 0, 0);
  }
}


void loop() {
  // Read from serial port over and over
  if (Serial.available() > 0) {
    arrivingdatabyte = Serial.read(); //It will read the incoming or arriving data byte
    Serial.print("data byte received: ");
    Serial.println(arrivingdatabyte);
    int o = -1;
 
    // DO STUFF WITH INCOMING DATA
    if (arrivingdatabyte == 97) { // 97 == "a"
      int wave_motors[] = {8,o,o,o, o,o,o,o, o,o,o,o, o,o,o,o};                    // CHOOSE THE PIN HERE. DEFAULT = Pin 0
      unsigned long wave_powers[] = {4090,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
      unsigned long on_time = 5000;                                                // turn on for 500 ms, or 0.5 seconds
      int num_on = 1;                                                              // tun on 1 motor (can do up to 16)
      run_motors(wave_motors, wave_powers, on_time, num_on);
    } 
  }
}
