#include <Arduino.h>
#include <painlessMesh.h>
#include "Wire.h"

//Mesh global and func
//#define MESH_SSID "WhateverYouLike"
#define MESH_SSID "sensortesting"
//THIS WIFI NAME IS INCORRECT TO NOT INTERFERE WITH MESH TESTING FOR NOW
#define MESH_PASSWORD "SomethingSneaky"
#define MESH_PORT 5555
#define MESG_DELAY_SEC 2
void receiveCallback(uint32_t from, String msg);
void parseSimpleJson(const char* jsonString);
void sendMessage();

//Sensor global and func
#define addr

//Mesh vars
Task taskSendMessage(TASK_SECOND * MESG_DELAY_SEC, TASK_FOREVER, &sendMessage);
Scheduler userSched; painlessMesh mesh;
uint32_t baseID = 0; int fail_count = 0;
const size_t bufferSize = 1024;

//Sensor vars
char dart[15]; char dlen=0;
uint8_t Data[100]={0}; uint8_t buff[4]={0};
uint16_t data, data1; uint16_t buff[100]={0};
float temp; float humid;

void setup() {
  //Mesh setup
  Serial.begin(9600); Serial.println("Wee woo clearing the way");
  mesh.setDebugMsgTypes(ERROR | DEBUG);
  mesh.init(MESH_SSID, MESH_PASSWORD, &userSched, MESH_PORT);
  mesh.onReceive(&receiveCallback);
  userSched.addTask(taskSendMessage); taskSendMessage.enable();

  //Sensor setup
  Wire.begin();
}

void loop() {
  //Mesh loop. DO NOT ADD DELAYS, it messes up the mesh
  mesh.update();

  //Sensor loop. This is commented out for now, as we will only need to
  //read the sensors when sending a message and not all the time
  /*
  readReg(0x00, buff, 4);
  data=buff[0]<<8|buff[1];
  data1=buff[2]<<8|buff[3];
  temp=((float)data*165/65535.0)-40.0;
  if(temp<70){
    Serial.println("Temperture is too low.");
  }
  humid=((float)data1/65535.0)*100;
  Serial.print(temp);
  Serial.print("C");
  Serial.println();
  */
}

//Sending message function
void sendMessage(){
  //Build message
  String msg = "Greenhouse 4\n";

  //Test if we have the base ID to send to
  if(mesh.isConnected(baseID)){
    mesh.sendSingle(baseID, msg); Serial.println("Sending... " + msg);
  }
  else{
    //If there is no base station, start to add to fails
    Serial.println("Base station is not connected");
    baseID = 0; fail_count += 1;
    //When there is too many fails, restart
    if (fail_count >= 5){
      Serial.println("Restarting..."); ESP.restart();
    }
  }

  //Scheduled every time
  taskSendMessage.setInterval( random(TASK_SECOND * MESG_DELAY_SEC, TASK_SECOND * 5));
}

//Receive message function, only used to obtain basestation ID
void receiveCallback(uint32_t from, String msg) {
  //Convert to char * and save as base station ID
  const char* json = msg.c_str();
  parseSimpleJson(json);
}

//Json unraveler, only used for basestationID
void parseSimpleJson(const char* jsonString)
{
//create the parsable json object
StaticJsonDocument<bufferSize> jsonDoc;
DeserializationError error = deserializeJson(jsonDoc, jsonString);
// catch the errors if there are any
if (error) {
Serial.printf("Failed to parse JSON: %s\n", error.c_str());
return;
}
//Set base ID
baseID = jsonDoc["basestation"];
// Output the json data
Serial.println("baseID: " + baseID);
}