#include <EEPROM.h>
#include "OpenMFA.h"
#include "Base64.h"
#include "cJson.h"
#include "JsonParser.h"
OpenMFA *device;

void setup()
{
    //  For Chrome, use 115200. For Serial, use 9600.
    Serial.begin(9600);
    delay(2000);
    //setBaudRate();
    
    Serial1.begin(115200);
    Serial.println("OpenMFA Device Boot.");
    
    //  Init
    *device = OpenMFA();
    //(*device).resetDevice(); Serial.println("OpenMFA Device Reset done.");   
    printDeviceInfo();
     
    //  Start loop
    Serial.println("OpenMFA Device Setup Complete.\n");
}

void setBaudRate(){
    Serial1.begin(38400);
    Serial1.print("AT");
    while (Serial1.available())
      Serial.print(Serial1.read());
    
    Serial.print("Setting namee");
    Serial1.print("AT+NAMEK-HC06"); // Set the name to JY-MCU-HC06
    while (Serial1.available())
      Serial.print(Serial1.read());
      
    Serial.print("Setting baud rate");
    Serial1.print("AT+BAUD8");
    delay(500);
    while (Serial1.available())
      Serial.print(Serial1.read());
    Serial.println("");
}


//  Variables in loop
char recvChar;
char buffer[256];
int buffer_i = 0;
JsonParser<16> parser;
    
void loop()
{
  if (Serial1.available() || Serial.available()){
    if (Serial.available())
      recvChar = Serial.read();
    if (Serial1.available())
      recvChar = Serial1.read();
      
    buffer[buffer_i] = recvChar;
    if (buffer_i > 254)
      buffer_i = 0;
    if (recvChar == '*'){
      buffer[buffer_i] = '\0';
      
      //  Process
      //cJSON* cmd = cJSON_Parse(buffer);
      JsonHashTable cmd = parser.parseHashTable(buffer);
      cJSON *response = cJSON_CreateObject();
      cmdToOpenMFAMethod(cmd, response); 
      char *output = cJSON_Print(response);
      Serial.println(output);
      Serial1.println(output);
      
      //  Free memory
      //cJSON_Delete(cmd);
      cJSON_Delete(response);
      free(output);
      
      buffer_i = 0;
    } else
      buffer_i++;
   }
}


void printDeviceInfo(){
    Serial.println("=== Device info ===");
    
    char* uuid;
    uuid = (*device).getUuid();
    Serial.print("Uuid: ");
    Serial.print(uuid);
    Serial.println();
    delete uuid;
    
    char* name;
    name = (*device).getName();
    Serial.print("Name: ");
    Serial.print(name);
    Serial.println();
    delete name;
    
    char* seed;
    seed = (*device).getSeed();
    Serial.print("Seed: ");
    Serial.print(seed);
    Serial.println();
    delete seed;
    
    char* hashedPassword;
    hashedPassword = (*device).getHashedPassword();
    Serial.print("Hashed Password: ");
    Serial.print(hashedPassword);
    Serial.println();
    delete hashedPassword;
    
    char* hashedPin;
    hashedPin = (*device).getHashedPin();
    Serial.print("Hashed Pin: ");
    Serial.print(hashedPin);
    Serial.println();
    delete hashedPin;
}


//  Call the OpenMFA device with a set of arguments
//cJSON* cmdToOpenMFAMethod(cJSON *cmd){
cJSON* cmdToOpenMFAMethod(JsonHashTable cmd, cJSON *response){
   //  Get method
   //char *method = cJSON_GetObjectItem(cmd, "queryType")->valuestring;
   char *method = cmd.getString("queryType");
   Serial.println(method);
   
   //  getUuid
   if (strcmp(method, "getUuid") == 0){
     char *uuid = (*device).getUuid();
     cJSON_AddStringToObject(response, "uuid", (*device).getUuid());
     delete uuid;
     
   //  getName
   } else if (strcmp(method, "getName") == 0){
     char *name = (*device).getName();
     cJSON_AddStringToObject(response, "name", name);
     delete name;
   
   //  setPassword
   } else if (strcmp(method, "setPassword") == 0){
     //char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *oldPassword = cmd.getString("oldPassword");
     //char *newPassword = cJSON_GetObjectItem(cmd, "newPassword")->valuestring;
     char *newPassword = cmd.getString("newPassword");
     if ((*device).setPassword(oldPassword, newPassword))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
   
   //  setPin
   } else if (strcmp(method, "setPin") == 0){
     //char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *oldPassword = cmd.getString("oldPassword");
     //char *newPin = cJSON_GetObjectItem(cmd, "newPin")->valuestring;
     char *newPin = cmd.getString("newPin");
     if ((*device).setPin(oldPassword, newPin))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
   
   //  setName
   } else if (strcmp(method, "setName") == 0){
     //char *oldPassword = cJSON_GetObjectItem(cmd, "oldPassword")->valuestring;
     char *oldPassword = cmd.getString("oldPassword");
     //char *newName = cJSON_GetObjectItem(cmd, "newName")->valuestring;
     char *newName = cmd.getString("newName");
     if ((*device).setName(oldPassword, newName))
       cJSON_AddTrueToObject(response, "success");
     else
       cJSON_AddFalseToObject(response, "success");
   
   //  getDomainSeed_E_Pin
   } else if (strcmp(method, "getDomainSeed_E_Pin") == 0){
     //char *domain = cJSON_GetObjectItem(cmd, "domain")->valuestring;
     char *domain = cmd.getString("domain");
     //char *pinNonce = cJSON_GetObjectItem(cmd, "pinNonce")->valuestring
     char *pinNonce = cmd.getString("pinNonce");
     
     char* domainSeed_E_Pin = (*device).getDomainSeed_E_Pin(domain, pinNonce);
     char* domainSeed = (*device).getDomainSeed(domain);
     char* uuid = (*device).getUuid();
     char* name = (*device).getName();
     
     cJSON_AddStringToObject(response, "domainSeed_E_Pin", domainSeed_E_Pin);
     cJSON_AddStringToObject(response, "domainSeedAnswer", domainSeed);
     cJSON_AddStringToObject(response, "uuid", uuid);
     cJSON_AddStringToObject(response, "name", name);
       
     delete domainSeed_E_Pin;
     delete domainSeed;
     delete uuid;
     delete name;
   
   //  getDomainOTP_E_Pin
   } else if (strcmp(method, "getDomainOTP_E_Pin") == 0){
     //char *domain = cJSON_GetObjectItem(cmd, "domain")->valuestring;
     char *domain = cmd.getString("domain");
     //char *pinNonce = cJSON_GetObjectItem(cmd, "pinNonce")->valuestring;
     char *pinNonce = cmd.getString("pinNonce");
     //char* timeIn5min = cJSON_GetObjectItem(cmd, "timeIn5min")->valuestring;
     char *timeIn5min = cmd.getString("timeIn5min");
     
     char* domainOTP_E_Pin = (*device).getDomainOTP_E_Pin(domain, pinNonce, timeIn5min);
     cJSON_AddStringToObject(response, "domainOTP_E_Pin", domainOTP_E_Pin);

     char* domainOTPAnswer = (*device).getDomainOTP(domain, timeIn5min);
     cJSON_AddStringToObject(response, "domainOTPAnswer", domainOTPAnswer);
     
     char* uuid = (*device).getUuid();
     cJSON_AddStringToObject(response, "uuid", uuid);
     
     char* name = (*device).getName();
     cJSON_AddStringToObject(response, "name", name);

     delete domainOTP_E_Pin;
     delete domainOTPAnswer;
     delete uuid;
     delete name;
   
   //  resetDevice
   } else if (strcmp(method, "resetDevice") == 0){
     (*device).resetDevice();
     cJSON_AddTrueToObject(response, "success");
   
   //  updateInfo
   } else if (strcmp(method, "updateInfo") == 0){
     //char *password = cJSON_GetObjectItem(cmd, "password")->valuestring;
     char *password = cmd.getString("password");
     //char *newName = cJSON_GetObjectItem(cmd, "newName")->valuestring;
     char *newName = cmd.getString("newName");
     //char *newPin = cJSON_GetObjectItem(cmd, "newPin")->valuestring;
     char *newPin = cmd.getString("newPin");
     //char *newPassword = cJSON_GetObjectItem(cmd, "newPassword")->valuestring;
     char *newPassword = cmd.getString("newPassword");
   
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
       
   //  getInfo
   } else if (strcmp(method, "getInfo") == 0){
     char* uuid = (*device).getUuid();
     char* name = (*device).getName();
     char* seed = (*device).getSeed();
     char* hashedPassword = (*device).getHashedPassword();
     char* hashedPin = (*device).getHashedPin();
     
     cJSON_AddStringToObject(response, "uuid", uuid);
     cJSON_AddStringToObject(response, "name", name);
     cJSON_AddStringToObject(response, "seed", seed);
     cJSON_AddStringToObject(response, "hashedPassword", hashedPassword);
     cJSON_AddStringToObject(response, "hashedPin", hashedPin);
     
     delete uuid;
     delete name;
     delete seed;
     delete hashedPassword;
     delete hashedPin;
   }
   
   //char *queryType = cJSON_GetObjectItem(cmd, "queryType")->valuestring;
   char *queryType = cmd.getString("queryType");
   //char *timestamp = cJSON_GetObjectItem(cmd, "timestamp")->valuestring;
   char *timestamp = cmd.getString("timestamp");
   cJSON_AddStringToObject(response, "queryType", queryType);
   cJSON_AddStringToObject(response, "timestamp", timestamp);

   return response;
}

int availableMemory() {
  int size = 1024*32; //  Max size
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);

  return size;
}
