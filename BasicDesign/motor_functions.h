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
    Responce();
    void init();
    void on();
    void print_info();
    
}; // don't forget the semicolon at the end of the class

void run_motors(uint8_t motors[], unsigned long powers[], unsigned long on_time);

void run_response(uint8_t wave_motors[][16], unsigned long wave_powers[][16], 
  unsigned long on_times[], unsigned long delay_times[]);
