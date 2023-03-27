//This file parses a JSON reponse and sets the value of the struct 

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
  // put your setup code here, to run once:
  Serial.begin(9600);
  //gameData *gameActions; 
  Serial.println(gameActions.motorAction);

  //TEST PARSED MESSAGE
  Serial.println("parsed request, sending FJ");
  parse_request("?1000", &gameActions);
  Serial.println(gameActions.motorAction);
  
  Serial.println("parsed request, sending LH");
  parse_request("?0001", &gameActions); 
  Serial.println(gameActions.motorAction);
  
  Serial.println("parsed request, sending nothing");
  parse_request("?0000", &gameActions);
  Serial.println(gameActions.motorAction);

  Serial.println("parsed request, sending HS");
  parse_request("?0100", &gameActions);
  Serial.println(gameActions.motorAction);

  Serial.println("parsed request, sending HL");
  parse_request("?0010", &gameActions);
  Serial.println(gameActions.motorAction);

  Serial.println("parsed request, sending FJ");
  parse_request("?1000", &gameActions);
  Serial.println(gameActions.motorAction);

}

void loop() {

}
