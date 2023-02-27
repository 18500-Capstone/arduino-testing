#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

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
