//go to website: http://capstonea8.wifi.local.cmu.edu/L
// taken from: https://docs.arduino.cc/tutorials/uno-wifi-rev2/uno-wifi-r2-hosting-a-webserver

#include <WiFiNINA.h>

char ssid[] = "CMU-DEVICE";             //  your network SSID (name) between the " "
char pass[] = "";      // your network password between the " "
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;      //connection status
WiFiServer server(80);            //server socket

WiFiClient client = server.available();

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

//void parse_request_temp(String req){
//  //declarations 
//  char startC = '?';
//  char endC = ' ';
//  char equalC = '='; 
//  
//  //getting indexes 
//  int indexOfStart = req.indexOf(startC);
//  int indexOfEnd = req.indexOf(endC); 
//  int indexOfEq;
//  
//  //getting the key and value string, setting the struct values 
//  String msg = req.substring(indexOfStart, indexOfEnd);
//  indexOfEq = msg.indexOf(equalC); 
//  String key = msg.substring(0, indexOfEq+1);
//  String valueStr = msg.substring(indexOfEq+1); //temp value
//  int value = valueStr.toInt();                 //converting temp string to int
//  gameData gameActions = {value, key}; 
//  Serial.println("PARSED VALUE: " + key + " " + value + "\n" );  
//}


void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  while (!Serial);
  
  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();
}

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
            
            int randomReading = analogRead(A1);
            client.print("Random reading from analog pin: ");
            client.print(randomReading);

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
        digitalWrite(ledPin, HIGH);        
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
