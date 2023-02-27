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

//=============================
// Create my responce class
// Help in creating a class: https://roboticsbackend.com/arduino-object-oriented-programming-oop/
// Help with arrays: https://www.arduino.cc/reference/en/language/variables/data-types/array/
//=============================
class Responce {
  private:
    static constexpr int num_waves = 4;
    static constexpr int max_motors = 4;
    unsigned long pause_timings[num_waves];
    unsigned long run_timings[num_waves];
    uint8_t wave_pins[num_waves][max_motors];
    unsigned long wave_powers[num_waves][max_motors];
    
  public:
    Responce(uint8_t wave1_pins[], uint8_t wave2_pins[], uint8_t wave3_pins[], 
      long wave1_pwr[], long wave2_pwr[], long wave3_pwr[], int buzz_timings, int pause_timings) {
//      this->wave1_pins = wave1_pins; // default is 4 different pins active because
//      this->wave2_pins = wave2_pins; // variable arrays get difficult
//      this->wave3_pins = wave3_pins; // if you want less than 4 pins, set to -1
//      this->wave1_power = wave1_pwr;
//      this->wave2_power = wave2_pwr;
//      this->wave3_power = wave3_pwr;
//      this->buzz_timings = buzz_timings;
//      this->pause_timings = pause_timings;
//      init();
    }

//    void init() {
//      pinMode(pin, OUTPUT);
//      // Always try to avoid duplicate code.
//      // initialize all pins
//      for(int i = 0; i < num_pins; i ++) {
//        pinMode(wave1_pins[i], OUTPUT);
//      }
//      
//      for(int i = 0; i < num_pins; i ++) {
//        pinMode(wave2_pins[i], OUTPUT);
//      }
//      
//      for(int i = 0; i < num_pins; i ++) {
//        pinMode(wave3_pins[i], OUTPUT);
//      }
//      off();
//    }

//    void on() {
//      digitalWrite(pin, HIGH);
//    }
//
//    void off() {
//      digitalWrite(pin, LOW);
//    }
}; // don't forget the semicolon at the end of the class



void setup() {
  Serial.begin(9600);
  Serial.println("16 channel PWM test!");

  pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  // if you want to really speed stuff up, you can go into 'fast 400khz I2C' mode
  // some i2c devices dont like this so much so if you're sharing the bus, watch
  // out for this!
  Wire.setClock(400000);
}

int arrivingdatabyte;
//uint8_t motors[];
//unsigned long powers[];
//unsigned long on_time;

struct tmpMotor{
  unsigned long on_time;
  uint8_t motors[16];
  unsigned long powers[16];
};


/* Run a batch of motors at some specified powers for some time
 *  IN: motors : an array of motors to run
 *  IN: powers : an array the same size of motors for the power of each motor
 *  IN: on_time : time to keep the motors on
 *  OUT : nothing -- controls I2C vibration motors
*/
void run_motors(uint8_t motors[], unsigned long powers[], unsigned long on_time) {
  unsigned long starttime = millis();
  unsigned long endtime = starttime;
  while ((endtime - starttime) <= on_time) {
    // Run all of the motors at their specified powers
    for (int i=0; i < sizeof(motors) - 1; i++) {
      if (motors[i] != -1) {
        pwm.setPWM(motors[i], 0, powers[i]);
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

//void run_response(uint8_t wave_motors[][16], unsigned long wave_powers[][16], 
//  unsigned long on_times[], unsigned long delay_times[]) {
//  // loop through and run each wave
//  for (int i=0; i < sizeof(wave_motors) - 1; i++) {
//    tmpMotor response1 = {10000, {7, 8}, {4090}};
//    Serial.print("about to run wave ");
//    Serial.println(i);
//    tmpMotor response1 = {wave_motors[i], wave_powers[i], on_times[i]};
//    run_motors(response1.motors, response1.powers, response1.on_time);
//
//    // delay for next wave
//    Serial.print("delaying for ")
//    Serial.print(delay_times[i]);
//    Serial.println(" seconds");
//    sleep(delay_times[i]);
//  }
//}

void run_response(uint8_t wave_motors[][16], unsigned long wave_powers[][16], 
  unsigned long on_times[], unsigned long delay_times[]) {
 
  // loop through and run each wave
  Serial.print("size of wave_motors: ");
  Serial.println(sizeof(wave_motors));
  for (int i=0; i < sizeof(wave_motors); i++) {
    tmpMotor response1 = {10000, {7, 8}, {4090}};
    Serial.print("about to run wave ");
    Serial.println(i);
    if (i == 0) {
      Serial.print("wave_motors[0]: ");
      Serial.println(wave_motors[0][0]);
      Serial.print("wave_powers[0]: ");
      Serial.println(wave_powers[0][0]);
      tmpMotor response1 = {wave_motors[i], wave_powers[i], on_times[i]};
      run_motors(response1.motors, response1.powers, response1.on_time);
    }
    else {
      tmpMotor response2 = {wave_motors[i], wave_powers[i], on_times[i]};
      run_motors(response2.motors, response2.powers, response2.on_time);
    }

    // delay for next wave
    Serial.print("delaying for ");
    Serial.print(delay_times[i]);
    Serial.println(" seconds");
    delay(delay_times[i]);
  }
}



void loop() {
  // Read from serial port over and over
  if (Serial.available() > 0) {
    arrivingdatabyte = Serial.read(); //It will read the incoming or arriving data byte
    Serial.print("data byte received: ");
    Serial.println(arrivingdatabyte);
    if (arrivingdatabyte == 97) { // 97 == "a"
      // Try to run function
//      uint8_t motors[] = {7};
//      unsigned long powers[] = {4090};
//      unsigned long on_time = 10000;
//      tmpMotor response1 = {10000, {7, 8}, {4090}};
//      Serial.println("about to run");
//      //run_motors(motors, powers,on_time);
//      run_motors(response1.motors, response1.powers, response1.on_time);

      // Try to run full response
      uint8_t wave_motors[][16] = {{7,2}, {1,1}, {7}};
      unsigned long wave_powers[][16] = {{4090, 23}, {6,0}, {4090}};
      unsigned long on_times[] = {10000, 200, 6000};
      unsigned long delay_times[] = {200, 5000};
      
      run_response(wave_motors, wave_powers, on_times, delay_times);
    }
  }
}

