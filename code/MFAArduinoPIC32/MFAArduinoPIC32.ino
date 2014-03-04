#include <EEPROM.h>
#include "OpenMFA.h"
#include "Base64.h"
#include "cJson.h"
OpenMFA *device;

void setup()
{
    //  For Chrome, use 115200. For Serial, use 9600.
    Serial.begin(9600);
    delay(2000);
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
    Serial.println("loop\n");
    delay(1000);
  return;
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
      cJSON* cmd = cJSON_Parse(buffer);
      
      cJSON *response = cmdToOpenMFAMethod(cmd); 
      char *output = cJSON_Print(response);
      Serial.println(output);
      
      //  Free memory
      cJSON_Delete(cmd);
      cJSON_Delete(response);
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
cJSON* cmdToOpenMFAMethod(cJSON *cmd){
   //  Get method
   char *method = cJSON_GetObjectItem(cmd, "queryType")->valuestring;
   Serial.println(method);
   
   //  Output
   cJSON *response = cJSON_CreateObject();
   
   //  getUuid
   if (strcmp(method, "getUuid") == 0){
     cJSON_AddStringToObject(response, "uuid", (*device).getUuid());
   
   //  getName
   } else if (strcmp(method, "getName") == 0){
     cJSON_AddStringToObject(response, "name", (*device).getName());
   
   //  setPassword
   } else if (strcmp(method, "setPassword") == 0){
     char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *newPassword = cJSON_GetObjectItem(cmd, "newPassword")->valuestring;
     if ((*device).setPassword(oldPassword, newPassword))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
       
     free(oldPassword);
     free(newPassword);
   
   //  setPin
   } else if (strcmp(method, "setPin") == 0){
     char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *newPin = cJSON_GetObjectItem(cmd, "newPin")->valuestring;
     if ((*device).setPin(oldPassword, newPin))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
       
     free(oldPassword);
     free(newPin);
   
   //  setName
   } else if (strcmp(method, "setName") == 0){
     char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *newName = cJSON_GetObjectItem(cmd, "newName")->valuestring;
     if ((*device).setName(oldPassword, newName))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
       
     free(oldPassword);
     free(newName);
   
   //  getDomainSeed_E_Pin
   } else if (strcmp(method, "getDomainSeed_E_Pin") == 0){
     char *domain = cJSON_GetObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = cJSON_GetObjectItem(cmd, "pinNonce")->valuestring;
     cJSON_AddStringToObject(response, "domainSeed_E_Pin", (*device).getDomainSeed_E_Pin(domain, pinNonce));
     cJSON_AddStringToObject(response, "domainSeedAnswer", (*device).getDomainSeed(domain));
     cJSON_AddStringToObject(response, "uuid", (*device).getUuid());
     cJSON_AddStringToObject(response, "name", (*device).getName());
       
     free(domain);
     free(pinNonce);
   
   //  getDomainOTP_E_Pin
   } else if (strcmp(method, "getDomainOTP_E_Pin") == 0){
     char *domain = cJSON_GetObjectItem(cmd, "domain")->valuestring;
     char *pinNonce = cJSON_GetObjectItem(cmd, "pinNonce")->valuestring;
     long timestamp = cJSON_GetObjectItem(cmd, "timestamp")->valueint;
     cJSON_AddStringToObject(response, "domainOTP_E_Pin", (*device).getDomainOTP_E_Pin(domain, pinNonce, timestamp));
     cJSON_AddStringToObject(response, "domainOTPAnswer", (*device).getDomainOTP(domain, timestamp));
     cJSON_AddStringToObject(response, "uuid", (*device).getUuid());
     cJSON_AddStringToObject(response, "name", (*device).getName());
       
     free(domain);
     free(pinNonce);
   
   //  resetDevice
   } else if (strcmp(method, "resetDevice") == 0){
     (*device).resetDevice();
     cJSON_AddTrueToObject(response, "success");
   
   //  updateInfo
   } else if (strcmp(method, "updateInfo") == 0){
     char *password = cJSON_GetObjectItem(cmd, "password")->valuestring;
     char *newName = cJSON_GetObjectItem(cmd, "newName")->valuestring;
     char *newPin = cJSON_GetObjectItem(cmd, "newPin")->valuestring;
     char *newPassword = cJSON_GetObjectItem(cmd, "newPassword")->valuestring;
   
     boolean valid = true;
     if (newName != NULL)
       valid = valid && ((*device).setName(password, newName));
     if (newPin != NULL)
       valid = valid && ((*device).setPin(password, newPin));
     if (newPassword != NULL)
       valid = valid && ((*device).setPassword(password, newPassword));
     
     if (valid)
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");    
       
     free(password);
     free(newName);    
     free(newPin);
     free(newPassword);                 
     
   //  getInfo
   } else if (strcmp(method, "getInfo") == 0){
     cJSON_AddStringToObject(response, "uuid", (*device).getUuid());
     cJSON_AddStringToObject(response, "name", (*device).getName());
     cJSON_AddStringToObject(response, "seed", (*device).getSeed());
     cJSON_AddStringToObject(response, "hashedPassword", (*device).getHashedPassword());
     cJSON_AddStringToObject(response, "hashedPin", (*device).getHashedPin());
   
   }
   
   cJSON_AddStringToObject(response, "queryType", cJSON_GetObjectItem(cmd, "queryType")->valuestring);
   cJSON_AddNumberToObject(response, "timestamp", cJSON_GetObjectItem(cmd, "timestamp")->valueint);
     
   free(method);
   return response;
}

