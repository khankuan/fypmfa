#include "OpenMFA.h"
#include "Base64.h"
#include <WProgram.h>
#include "sha1.c"

/*
 * OpenMFA constructor
 */
OpenMFA::OpenMFA()
{
}



/*
 * OpenMFA_Device get uuid
 */
char* OpenMFA::getUuid()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[sizeof(OpenMFA_data.uuid)+1];
    memcpy(output, OpenMFA_data.uuid, sizeof(OpenMFA_data.uuid));
    output[sizeof(OpenMFA_data.uuid)] = '\0';
    return output;
}



/*
 * OpenMFA get name
 */
char* OpenMFA::getName()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[sizeof(OpenMFA_data.name)+1];
    memcpy(output, OpenMFA_data.name, sizeof(OpenMFA_data.name));
    output[sizeof(OpenMFA_data.name)] = '\0';
    return output;
}



/*
 * OpenMFA set password of this device
 */
bool OpenMFA::setPassword(char* inputOldPassword, char* inputNewPassword)
{
    if (strlen(inputOldPassword) == 0)
      return false;
  
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    char* inputOldPasswordHashed = hash(inputOldPassword);
    char* inputNewPasswordHashed = hash(inputNewPassword);
    
    //  Check if old password == current password
    if (memcmp(inputOldPasswordHashed, OpenMFA_data.hashedPassword, strlen(inputOldPasswordHashed)) != 0)
        return false;
        
    //  Set new password
    base64_encode(OpenMFA_data.hashedPassword, inputNewPasswordHashed, strlen(inputNewPasswordHashed));
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
    delete inputOldPasswordHashed;
    delete inputNewPasswordHashed;
    return true;
}



/*
 * OpenMFA set pin of this device
 */
bool OpenMFA::setPin(char* inputPassword, char* inputNewPin)
{
    if (strlen(inputPassword) == 0)
      return false;
  
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    char* inputPasswordHashed = hash(inputPassword);
    char* inputNewPinHashed = hash(inputNewPin);
    
    //  Check if old password == current password
    if (memcmp(inputPasswordHashed, OpenMFA_data.hashedPassword, strlen(inputPasswordHashed)) != 0)
        return false;
        
    //  Set new password
    base64_encode(OpenMFA_data.hashedPin, inputNewPinHashed, strlen(inputNewPinHashed));
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
    delete inputPasswordHashed;
    delete inputNewPinHashed;
    return true;
}



/*
 * OpenMFA set name of this device
 */
bool OpenMFA::setName(char* inputPassword, char* inputNewName)
{
    if (strlen(inputPassword) == 0)
      return false;
  
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    char* inputPasswordHashed = hash(inputPassword);
    
    //  Check if old password == current password
    if (memcmp(inputPasswordHashed, OpenMFA_data.hashedPassword, strlen(inputPasswordHashed)) != 0)
        return false;
        
    //  Store
    strcpy(OpenMFA_data.name, inputNewName);  //  pin same as password for default
    delete inputPasswordHashed;
    return true;
}



/*
 * OpenMFA get the Seed_Domain of a domain name, in base 64 and encrypted with pin
 */
char* OpenMFA::getDomainSeed_E_Pin(char* domain, char* pinNonce)
{
    char* domainSeed = getDomainSeed(domain);
    
    //  obtain h(pin, pinNonce)
    char *oneTimePin = getOneTimePin(pinNonce);
    
    //  Xor to encrypt
    char* result = xorBase64(domainSeed, oneTimePin);
    delete domainSeed;
    delete oneTimePin;
    return result;
}



/*
 * OpenMFA get the OTP of a domain name in base 64
 */
char* OpenMFA::getDomainOTP_E_Pin(char* domain, char* pinNonce, char* timeIn5min){
    
    char* domainOTP = getDomainOTP(domain, timeIn5min);
    
    //  obtain h(pin, pinNonce)
    char *oneTimePin = getOneTimePin(pinNonce);
    
    //  Xor to encrypt
    char* result = xorBase64(domainOTP, oneTimePin);
    delete domainOTP;
    delete oneTimePin;
    return result;
}



/*
 * OpenMFA reset
 *
 */
void OpenMFA::resetDevice()
{
    OpenMFA_data OpenMFA_data;
    
    //  Memset
    memset (OpenMFA_data.uuid,0,32);
    memset (OpenMFA_data.name,0,32);
    memset (OpenMFA_data.seed,0,32);
    memset (OpenMFA_data.hashedPassword,0,32);
    memset (OpenMFA_data.hashedPin,0,32);

    //  Flash data
    //randomSeed(analogRead(0));
    //unsigned long num = random(0, 100000000);
    //unsigned long numName = num % 1000;
    unsigned long num = rand();
    unsigned long numName = rand();
    char numS[32];
    char numNameS[32];
    ltoa(num, numS, 10);
    ltoa(numName, numNameS, 10);
    char *uuid = concat("UUID-", numS);
    char *name = concat("Name-", numS);
    
    memcpy(OpenMFA_data.uuid, uuid, 32);
    memcpy(OpenMFA_data.name, name, 32);
    
    delete uuid;
    delete name;
    
    //  Seed
    char *randomBytes = generateRandomBytes(32);
    
    char *seed = hash(randomBytes);
    
    memcpy(OpenMFA_data.seed, seed, strlen(seed));
    delete randomBytes;
    delete seed;
    
    //  Password and pin
    char *hashedPassword = hash("default");
    memcpy(OpenMFA_data.hashedPassword, hashedPassword, 32);
    delete hashedPassword;
    
    char *hashedPin = hash("default");
    memcpy(OpenMFA_data.hashedPin, hashedPin, 32);
    delete hashedPin;
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
}


/*  Test Methods */

char* OpenMFA::getSeed(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    char *result = new char[sizeof(OpenMFA_data.seed)+1];
    memcpy(result, OpenMFA_data.seed, sizeof(OpenMFA_data.seed));
    result[sizeof(OpenMFA_data.seed)] = '\0';
    return result;
}

char* OpenMFA::getHashedPassword(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    char *result = new char[sizeof(OpenMFA_data.hashedPassword)+1];
    memcpy(result, OpenMFA_data.hashedPassword, sizeof(OpenMFA_data.hashedPassword));
    result[sizeof(OpenMFA_data.hashedPassword)] = '\0';
    return result;
}

char* OpenMFA::getHashedPin(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    char *result = new char[sizeof(OpenMFA_data.hashedPin)+1];
    memcpy(result, OpenMFA_data.hashedPin, sizeof(OpenMFA_data.hashedPin));
    result[sizeof(OpenMFA_data.hashedPin)] = '\0';
    return result;
}



/*
 * OpenMFA get the Seed_Domain of a domain name in base 64
 */
char* OpenMFA::getDomainSeed(char* domain){
    char* seed = getSeed();
    
    char* domainSeed = concat(seed, domain);
    char* result = hash(domainSeed);
    delete domainSeed;
    delete seed;
    
    return result;
}



/*
 * OpenMFA get the OTP of a domain name in base 64
 */
char* OpenMFA::getDomainOTP(char* domain, char* timeIn5min){
    char* domainSeed = getDomainSeed(domain);
    
    char* OTP = concat(domainSeed, timeIn5min);
    char* result = hash(OTP);
    
    delete domainSeed;
    delete OTP;
    return result;
}


//  To get the one time pin based on the pin and nonce
char* OpenMFA::getOneTimePin(char* pinNonce){
   char* pin = getHashedPin();
   
   char* oneTimePin = concat(pin, pinNonce);
   char* result = hash(oneTimePin);
   delete oneTimePin;
   delete pin;
   
   char* output = new char[strlen(result)+1];
   memcpy(output, result, strlen(result));
   output[strlen(result)] = '\0';
   
   delete result;
   
   return output;
}



/*
 * Hash function
 */
char* OpenMFA::hash(char *s)
{
    sha1nfo sha;
    sha1_init(&sha);
    sha1_write(&sha, s, strlen(s));
    char *result = (char*) sha1_result(&sha);
    
    //  encode to base64
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    char resultBase64[base64_length];
    base64_encode(resultBase64, result, SHA1_DEFAULT_KEY_LENGTH);
    
    char *output = new char[base64_length+1];
    memcpy(output, resultBase64, base64_length);
    output[base64_length] = '\0';
    
    return output;
}



/*
 * Xor function for base64 strings
 */
char* OpenMFA::xorBase64(char* msg, char* key)
{
    //  Convert to binary
    int base64_length_msg = strlen(msg);
    int binary_length_msg = base64_dec_len(msg, base64_length_msg);
    char *decodedMsg = new char[binary_length_msg+1];
    base64_decode(decodedMsg, msg, base64_length_msg);
    decodedMsg[binary_length_msg] = '\0';
    
    int base64_length_key = strlen(key);
    int binary_length_key = base64_dec_len(key, base64_length_key);
    char *decodedKey = new char[binary_length_key+1];
    base64_decode(decodedKey, key, base64_length_key);
    decodedKey[binary_length_key] = '\0';
    
    //  Actual XOR
    char *decodedResult = new char[binary_length_msg+1];
    for (int i = 0 ; i < 20; i++)
      decodedResult[i] = decodedMsg[i] ^ decodedKey[i % binary_length_key];
    decodedResult[binary_length_msg] = '\0';
      
    //  Encode result
    int base64_length = base64_enc_len(binary_length_msg);
    char *output = new char[base64_length+1];
    base64_encode(output, decodedResult, binary_length_msg);
    output[base64_length] = '\0';

    delete decodedMsg;
    delete decodedKey;
    delete decodedResult;
    
    return output;
}



/*
 * OpenMFA generate random 32 Bytes
 */
char* OpenMFA::generateRandomBytes(int bytes)
{
    char *bits = new char[bytes];
    for (int i = 0; i < bytes; i++)
    bits[i] = rand() % 256;
    return bits;
}



char* OpenMFA::concat(char *s1, char *s2)
{
    char *result = new char[strlen(s1)+strlen(s2)+1];//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    result[strlen(s1) + strlen(s2)] = '\0';
    return result;
}
