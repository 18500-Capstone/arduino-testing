
//DECLARING MOTOR PINS 
const int motorPin = 3; //pwm 

void setup()
{
  pinMode(motorPin, OUTPUT);
}

void loop()
{
  analogWrite(motorPin, 127);      //vibrate at half power 
  delay(1000);  // delay one second
  analogWrite(motorPin, 0);
  //digitalWrite(motorPin, LOW);  //stop vibrating
  delay(1000); //wait 50 seconds.
}
