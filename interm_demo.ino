//go to website: http://capstonea8.wifi.local.cmu.edu/L
// taken from: https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-hosting-a-webserver

#include <WiFiNINA.h>

char ssid[] = "CMU-DEVICE";             //  your network SSID (name) between the " "
char pass[] = "";      // your network password between the " "
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();


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


int ledPin = 3;

//Holds the current actions and (eventually) health level amount 
typedef struct {

  //FOR THE ACTUAL GAME DATA 
  String motorAction; //one of 6 values: {FJ, HS, HL, LH, -}
  String lightShowAction; //same 6 values: {FJ, HS, HL, LH, -} 
  //int healthLevel;  //holds number in range 0-25
} gameData;

gameData gameActions; //declaring instance of struct

//parses JSON request and sets the struct values accordingly 
void parse_request(String req, gameData *ga){

 //declarations
 //char delimitC = ",";
 String motorActionStr, lightActionStr = "";
 req = req.substring(1); //get rid of first character
 
 //getting index 
 //int indexOfComma = req.indexOf(delimitC);

 //setting motorAction and light substring
  motorActionStr = req; //1 hot vector "0010" FJ, HS, HL, LH
 
 //forceful jump state 1000
 if(motorActionStr.substring(0,1) == "1"){ //index corresponds to FJ
    motorActionStr = "FJ";
    ga->motorAction = motorActionStr;
    ga->lightShowAction = motorActionStr;
    return; 
 }
 
 ////Hit by a small enemy state 0100
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HS"; 
    ga->motorAction = motorActionStr;
    ga->lightShowAction = motorActionStr;
    return; 
 }
 
 //Hit by a large enemy state 0010
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HL";
    ga->motorAction = motorActionStr;
    ga->lightShowAction = motorActionStr;
    return; 
 }
 //low health state 0001
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "LH";
    ga->motorAction = motorActionStr;
    ga->lightShowAction = motorActionStr;
    return; 
 }
 //default state (0000) where no relevant action is occuring
 if(motorActionStr.substring(0,1) == "0"){
    motorActionStr = "-";
    ga->motorAction = motorActionStr;
    ga->lightShowAction = motorActionStr;
    return; 
 } 
}

void setup() {
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  while (!Serial);
  
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();
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


void loop() {
  client = server.available();

  if (client) {
    printWEB();
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  
  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  //printMacAddress(mac);
  
  //void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
//}
  
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi() {
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWEB() {

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //concatonate into a buffer, parse the buffer between ? and \space 
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character (we've finished seeing a line)
          //parse_request_temp(currentLine);
          //if the first character of the line starts with ?, parse the request and set the struct fields 
          if(currentLine.substring(0,1) == '?') parse_request(currentLine, &gameActions);

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
           
            //create the buttons
            client.print("Click <a href=\"/H\">here</a> turn the LED on<br>");
            client.print("Click <a href=\"/L\">here</a> turn the LED off<br><br>");
            
//            int randomReading = analogRead(A1);
//            client.print("Random reading from analog pin: ");
//            client.print(randomReading);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /H")) {
        //digitalWrite(ledPin, HIGH); 
        cascade(colorArray, 100, 75);
        strip.show();
        //run_response1();        
        }
        if (currentLine.endsWith("GET /L")) {
        digitalWrite(ledPin, LOW);       
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
