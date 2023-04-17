//Description: This file parses a JSON reponse and sets the value of the struct 

//Holds the current actions and (eventually) health level amount 
typedef struct {

  //FOR THE ACTUAL GAME DATA 
  int action; //one of 5 values: {FJ, HS, HL, LH, -}->{0, 1, 2, 3, 4}
  int desiredMotorIntensity; 
  int healthLevel;  //holds number in range 0-100
} gameData;

gameData gameActions; //declaring instance of struct 

//parses JSON request and sets the struct values accordingly 
void parse_request(String req, gameData *ga){

 //declarations
 char delimitC = ',';
 String motorActionStr, healthNumStr, motorStrengthStr = "";
 req = req.substring(1); //get rid of first character
 
 //getting index 
 int indexOf1stComma = req.indexOf(delimitC);
 int indexOfLastComma = req.lastIndexOf(delimitC); 
 int indexOfLastChar = req.length(); 
 
 //isolating motorAction, health num, and motor strength substring
 motorActionStr = req.substring(0,indexOf1stComma); //1 hot vector "0010" FJ, HS, HL, LH
 healthNumStr = req.substring(indexOf1stComma+1, indexOfLastComma);
 //motorStrengthStr = req.substring(indexOfLastComma, indexOfLastChar);
 motorStrengthStr = req.substring(indexOfLastComma+1);
  
 //converting healthNumStr and motorStrengthStr to int and setting the struct values 
 if(healthNumStr == "NaN"){
  ga->healthLevel = 100;
 }
 else{
  ga->healthLevel = healthNumStr.toInt();
 }
 ga->desiredMotorIntensity = motorStrengthStr.toInt(); 
 
  
 //forceful jump state 1000
 if(motorActionStr.substring(0,1) == "1"){ //index corresponds to FJ
    motorActionStr = "FJ";
    ga->action = 0;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 
 ////Hit by a small enemy state 0100
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HS"; 
    ga->action = 1;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 
 //Hit by a large enemy state 0010
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "HL";
    ga->action = 2;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 //low health state 0001
 motorActionStr = motorActionStr.substring(1);
 if(motorActionStr.substring(0,1) == "1"){
    motorActionStr = "LH";
    ga->action = 3;
    //ga->lightShowAction = motorActionStr;
    return; 
 }
 //default state (0000) where no relevant action is occuring
 if(motorActionStr.substring(0,1) == "0"){
    motorActionStr = "-";
    ga->action = 4;
    //ga->lightShowAction = motorActionStr;
    return; 
 } 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //gameData *gameActions; 
  //Serial.println(gameActions.action);

  //TEST PARSED MESSAGE
//  Serial.println("parsed request, sending FJ with health 87, intensity 40");
//  parse_request("?1000,87,40\n", &gameActions);
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//
//  Serial.println("parsed request, sending FJ with health NaN, intensity 25 (should see health==100)");
//  parse_request("?1000,NaN,25\n", &gameActions); 
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//  
//  Serial.println("parsed request, sending LH with health 32, intensity 100");
//  parse_request("?0001,32,100\n", &gameActions); 
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//  
//  Serial.println("parsed request, sending nothing with health 20, intensity 0");
//  parse_request("?0000,20,0\n", &gameActions);
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//  
//  Serial.println("parsed request, sending HS with health 55, intensity 20");
//  parse_request("?0100,55,20\n", &gameActions);
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//
//  Serial.println("parsed request, sending HL with health 99, intensity 78");
//  parse_request("?0010,99,78\n", &gameActions);
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//  
//  Serial.println("parsed request, sending FJ with health 12, intensity 66");
//  parse_request("?1000,12,66\n", &gameActions);
//  Serial.println(gameActions.action);
//  Serial.println(gameActions.healthLevel);
//  Serial.println(gameActions.desiredMotorIntensity);
//  
}

//void loop() {
//  while (Serial.available() == 0) {}     //wait for data available
//  Serial.println("SERIAL AVAILABLE");
//  String currentLine = Serial.readString();  //read until timeout
//  
//  if(currentLine.charAt(currentLine.length()-1) == '\n'){
//    parse_request(currentLine, &gameActions);
//    Serial.println("PARSED!");
//    Serial.println(gameActions.action);
//    Serial.println(gameActions.healthLevel);
//    Serial.println(gameActions.desiredMotorIntensity);
//  }
//
//}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    int red = Serial.parseInt();
    int green = Serial.parseInt();
    int blue = Serial.parseInt();
    //Serial.println(red.toInt() + " "  + green.toInt() + " " + blue.toInt());
  }
}
