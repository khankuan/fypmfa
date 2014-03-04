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


//  Variables in loop
char recvChar;
char buffer[256];
int buffer_i = 0;

    
void loop()
{
  if (Serial.available()){//check if there's any data sent from the remote bluetooth shield
    recvChar = Serial.read();
    //Serial.print(recvChar); //  Do not print out, might be slow.
    buffer[buffer_i] = recvChar;
    if (buffer_i > 254)
      buffer_i = 0;
    if (recvChar == '*'){
      buffer[buffer_i] = '\0';
      
      //  Process
      Serial.println(buffer);
      aJsonObject* cmd = aJson.parse(buffer);
      
      aJsonObject *response = cmdToOpenMFAMethod(cmd); 
      char *output = aJson.print(response);
      Serial.println(output);
      
      //  Free memory
      aJson.deleteItem(cmd);
      aJson.deleteItem(response);
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
    
    output = (*device).getSeed();
    Serial.print("Seed: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getHashedPassword();
    Serial.print("Hashed Password: ");
    Serial.print(output);
    Serial.println();
    free(output);
    
    output = (*device).getHashedPin();
    Serial.print("Hashed Pin: ");
    Serial.print(output);
    Serial.println();
    free(output);
}


//  Call the OpenMFA device with a set of arguments
aJsonObject* cmdToOpenMFAMethod(aJsonObject *cmd){
   //  Get method
   char *method = aJson.getObjectItem(cmd, "queryType")->valuestring;
   Serial.println(method);
   
   //  Output
   aJsonObject *response = aJson.createObject();
   
   //  getUuid
   if (strcmp(method, "getUuid") == 0){
     aJson.addStringToObject(response, "uuid", (*device).getUuid());
   
   //  getName
   } else if (strcmp(method, "getName") == 0){
     aJson.addStringToObject(response, "name", (*device).getName());
   
   //  setPassword
   } else if (strcmp(method, "setPassword") == 0){
     char *oldPassword = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *newPassword = aJson.getObjectItem(cmd, "newPassword")->valuestring;
     if ((*device).setPassword(oldPassword, newPassword))
       aJson.addTrueToObject(response, "success");
     else
       aJson.addFalseToObject(response, "success");
       
     free(oldPassword);
     free(newPassword);
   
   //  setPin
   } else if (strcmp(method, "setPin") == 0){
     char *oldPassword = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *newPin = aJson.getObjectItem(cmd, "newPin")->valuestring;
     if ((*device).setPin(oldPassword, newPin))
       aJson.addTrueToObject(response, "success");
     else
       aJson.addFalseToObject(response, "success");
       
     free(oldPassword);
     free(newPin);
   
   //  setName
   } else if (strcmp(method, "setName") == 0){
     char *oldPassword = aJson.getObjectItem(cmd, "oldPassword")->valuestring;
     char *newName = aJson.getObjectItem(cmd, "newName")->valuestring;
     if ((*device).setName(oldPassword, newName))
       aJson.addTrueToObject(response, "success");
     else
       aJson.addFalseToObject(response, "success");
       
     free(oldPassword);
     free(newName);
   
   //  getDomainSeed_E_Pin
   } else if (strcmp(method, "getDomainSeed_E_Pin") == 0){
     char *domain = aJson.getObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = aJson.getObjectItem(cmd, "pinNonce")->valuestring;
     aJson.addStringToObject(response, "domainSeed_E_Pin", (*device).getDomainSeed_E_Pin(domain, pinNonce));
     aJson.addStringToObject(response, "domainSeedAnswer", (*device).getDomainSeed(domain));
     aJson.addStringToObject(response, "uuid", (*device).getUuid());
     aJson.addStringToObject(response, "name", (*device).getName());
       
     free(domain);
     free(pinNonce);
   
   //  getDomainOTP_E_Pin
   } else if (strcmp(method, "getDomainOTP_E_Pin") == 0){
     char *domain = aJson.getObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = aJson.getObjectItem(cmd, "pinNonce")->valuestring;
     long timestamp = aJson.getObjectItem(cmd, "timestamp")->valueint;
     aJson.addStringToObject(response, "domainOTP_E_Pin", (*device).getDomainOTP_E_Pin(domain, pinNonce, timestamp));
     aJson.addStringToObject(response, "domainOTPAnswer", (*device).getDomainOTP(domain, timestamp));
     aJson.addStringToObject(response, "uuid", (*device).getUuid());
     aJson.addStringToObject(response, "name", (*device).getName());
       
     free(domain);
     free(pinNonce);
   
   //  resetDevice
   } else if (strcmp(method, "resetDevice") == 0){
     (*device).resetDevice();
     aJson.addTrueToObject(response, "success");
   
   //  updateInfo
   } else if (strcmp(method, "updateInfo") == 0){
     char *password = aJson.getObjectItem(cmd, "password")->valuestring;
     char *newName = aJson.getObjectItem(cmd, "newName")->valuestring;
     char *newPin = aJson.getObjectItem(cmd, "newPin")->valuestring;
     char *newPassword = aJson.getObjectItem(cmd, "newPassword")->valuestring;
   
     boolean valid = true;
     if (newName != NULL)
       valid = valid && ((*device).setName(password, newName));
     if (newPin != NULL)
       valid = valid && ((*device).setPin(password, newPin));
     if (newPassword != NULL)
       valid = valid && ((*device).setPassword(password, newPassword));
     
     if (valid)
       aJson.addTrueToObject(response, "success");
     else
       aJson.addFalseToObject(response, "success");    
       
     free(password);
     free(newName);    
     free(newPin);
     free(newPassword);                 
     
   //  getInfo
   } else if (strcmp(method, "getInfo") == 0){
     aJson.addStringToObject(response, "uuid", (*device).getUuid());
     aJson.addStringToObject(response, "name", (*device).getName());
     aJson.addStringToObject(response, "seed", (*device).getSeed());
     aJson.addStringToObject(response, "hashedPassword", (*device).getHashedPassword());
     aJson.addStringToObject(response, "hashedPin", (*device).getHashedPin());
   
   }
   
   aJson.addStringToObject(response, "queryType", aJson.getObjectItem(cmd, "queryType")->valuestring);
   aJson.addNumberToObject(response, "timestamp", aJson.getObjectItem(cmd, "timestamp")->valueint);
     
   free(method);
   return response;
}

