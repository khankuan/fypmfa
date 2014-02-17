/*
getUuid?
getName?
setName,default,My Wearable Device?
setPassword,default,newpassword?
setPin,newpassword,newpin?
getSeedDomain64_E_Pin,google.com,12123123?
getOTP64_E_Pin,google.com,2308191,12123123?
*/

#include <EEPROM.h>
#include "OpenMFA.h"
#include <SoftwareSerial.h>
#include "Base64.h"
#include "aJson.h"
OpenMFA *device;

#define RxD 7
#define TxD 8
#define LED_PIN 13
SoftwareSerial blueToothSerial(RxD,TxD);

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    //  For Chrome, use 115200. For Serial, use 9600.
    Serial.begin(115200);
    //setBaudRate();
    Serial.println("OpenMFA Device Boot.");
    
    //  Init
    *device = OpenMFA();
    (*device).resetDevice(); Serial.println("OpenMFA Device Reset done.");   
    //printDeviceInfo();
     
    //  Command tests
    //char * google_seedDomain = (*device).getSeedDomain64_E_Pin("google.com", 123114L);
    //char* one_time_pin = "Ls17A9aY0P058+Qq32vqwF+wrMA=";
    //char *OTP_E_Pin = (*device).getOTP64_E_Pin("google.com", 23011540L, 23011540L);
    
    //  Start loop
    Serial.println("OpenMFA Device Setup Complete.\n");
}

void setBaudRate()
{
    Serial.println('$$$');
    delay(1);
    Serial.println('SU,11');
    delay(1);
    Serial.println('---');
    delay(1);
    //Serial.begin(19200);
    //delay(1);
    Serial.println('---');
}

//  Variables in loop
char recvChar;
char buffer[256];
int buffer_i = 0;

    
void loop()
{
//  digitalWrite(LED_PIN, HIGH);
//  delay(300);
//  digitalWrite(LED_PIN, LOW);
//  delay(300);
//  
//  while (Serial.available()){//check if there's any data sent from the remote bluetooth shield
//    recvChar = Serial.read();
//    Serial.print(recvChar);
//    Serial.print(",");
//  }

  if (Serial.available()){//check if there's any data sent from the remote bluetooth shield
    recvChar = Serial.read();
    //Serial.print(recvChar); //  Do not print out, might be slow.
    buffer[buffer_i] = recvChar;
    if (buffer_i > 254)
      buffer_i = 0;
    if (recvChar == '*'){
      buffer[buffer_i+1] = '\0';
      char* msg = new char[buffer_i+1];
      memcpy(msg, buffer, buffer_i+1);
      
      //  Process
      Serial.println(msg);
      aJsonObject* cmd = aJson.parse(msg);
      
      char *output = new char[256];
      argsToOpenMFAMethod(cmd, output);
      Serial.print(aJson.getObjectItem(cmd, "queryType")->valuestring);

      
      //  Free memory
      free(cmd);
      free(output);
      
      buffer_i = 0;
    } else
      buffer_i++;
   }
}


void printDeviceInfo(){
    Serial.println("=== Device info ===");
    
    char* output;
    
    output = (*device).getUuid();
    Serial.print("Uuid: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getName();
    Serial.print("Name: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getSeed64();
    Serial.print("Seed: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getHashedPassword64();
    Serial.print("Hashed Password: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getHashedPin64();
    Serial.print("Hashed Pin: ");
    Serial.print(output);
    Serial.println();
    free(output);
}



//  Convert a string command to arguments
int commandToArgs(char* command, char** args){
  int argc = 0;
  
  //  Max 10 arguments
  args[0] = command;
  argc += 1;
  
  //  Change to string
  int len = strlen(command);
  for (int i = 0; i < len; i++){
    if (command[i]  == ','){
      command[i] = '\0';
      args[argc] = command+i+1;
      argc++;
    }
  }
  
  return argc;
}


//  Call the OpenMFA device with a set of arguments
void argsToOpenMFAMethod(aJsonObject *cmd, char* output){
   //  Get method
   char *method = aJson.getObjectItem(cmd, "queryType")->valuestring;
   Serial.println(method);
   return;
   //  getUuid
   if (strcmp(method, "getUuid") == 0){
     strcpy(output,(*device).getUuid());
   
   //  getName
   } else if (strcmp(method, "getName") == 0){
     strcpy(output,(*device).getName());
   
   //  setPassword
   } else if (strcmp(method, "setPassword") == 0){
     char *old_password = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *new_password = aJson.getObjectItem(cmd, "newPassword")->valuestring;
     if ((*device).setPassword(old_password, new_password))
       strcpy(output,"OK");
     else
       strcpy(output,"FAIL");
   
   //  setPin
   } else if (strcmp(method, "setPin") == 0){
     char *old_password = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *new_pin = aJson.getObjectItem(cmd, "newPin")->valuestring;
     if ((*device).setPin(old_password, new_pin))
       strcpy(output,"OK");
     else
       strcpy(output,"FAIL");
   
   //  setName
   } else if (strcmp(method, "setName") == 0){
     char *old_password = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *new_name = aJson.getObjectItem(cmd, "newName")->valuestring;
     if ((*device).setName(old_password, new_name))
       strcpy(output,"OK");
     else
       strcpy(output,"FAIL");
   
   //  getSeedDomain64_E_Pin
   } else if (strcmp(method, "getSeedDomain64_E_Pin") == 0){
     char *domain = aJson.getObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = aJson.getObjectItem(cmd, "pinNonce")->valuestring;
     strcpy(output,(*device).getSeedDomain64_E_Pin(domain, atol(pinNonce)));
   
   //  getOTP64_E_Pin
   } else if (strcmp(method, "getOTP64_E_Pin") == 0){
     char *domain = aJson.getObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = aJson.getObjectItem(cmd, "pinNonce")->valuestring;
     char *timestamp = aJson.getObjectItem(cmd, "timestamp")->valuestring;
     strcpy(output,(*device).getOTP64_E_Pin(domain, atol(pinNonce), atol(timestamp)));
   
   //  resetDevice
   } else if (strcmp(method, "resetDevice") == 0){
     (*device).resetDevice();
     strcpy(output,"OK");
   
   //  Invalid
   } else
     strcpy(output,"INVALID");
}

