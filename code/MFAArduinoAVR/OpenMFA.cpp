#include "OpenMFA.h"
#include "Base64.h"

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
    char *output = new char[32];
    strcpy(output, OpenMFA_data.uuid);
    return output;
}



/*
 * OpenMFA get name
 */
char* OpenMFA::getName()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[32];
    strcpy(output, OpenMFA_data.name);
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
    free(inputOldPasswordHashed);
    free(inputNewPasswordHashed);
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
    free(inputPasswordHashed);
    free(inputNewPinHashed);
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
    free(inputPasswordHashed);
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
    free(domainSeed);
    free(oneTimePin);
    return result;
}



/*
 * OpenMFA get the OTP of a domain name in base 64
 */
char* OpenMFA::getDomainOTP_E_Pin(char* domain, char* pinNonce, long timeInMS){
    
    char* domainOTP = getDomainOTP(domain, timeInMS);
    
    //  obtain h(pin, pinNonce)
    char *oneTimePin = getOneTimePin(pinNonce);
    
    //  Xor to encrypt
    char* result = xorBase64(domainOTP, oneTimePin);
    free(domainOTP);
    free(oneTimePin);
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
    randomSeed(analogRead(0));
    unsigned long num = random(0, 100000000);
    unsigned long numName = num % 1000;
    
    ("UUID-" + String(num)+"\0").toCharArray(OpenMFA_data.uuid, 32);
    ("Name-" + String(numName)+"\0").toCharArray(OpenMFA_data.name, 32);
    
    //  Seed
    char *randomBytes = generateRandomBytes(32);
    
    char *seed = hash(randomBytes);
    memcpy(OpenMFA_data.seed, seed, strlen(seed));
    
    free(randomBytes);
    free(seed);
    
    //  Password and pin
    char *hashedPassword = hash("default");
    
    memcpy(OpenMFA_data.hashedPassword, hashedPassword, sizeof(OpenMFA_data.hashedPassword));
    free(hashedPassword);
    
    char *hashedPin = hash("default");
    memcpy(OpenMFA_data.hashedPin, hashedPin, sizeof(OpenMFA_data.hashedPin));
    free(hashedPin);
    Serial.println("Asd");
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
}


/*  Test Methods */

char* OpenMFA::getSeed(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    char *result = new char[strlen(OpenMFA_data.seed)];
    memcpy(result, OpenMFA_data.seed, strlen(OpenMFA_data.seed));
    return result;
}

char* OpenMFA::getHashedPassword(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    char *result = new char[strlen(OpenMFA_data.hashedPassword)];
    memcpy(result, OpenMFA_data.hashedPassword, strlen(OpenMFA_data.hashedPassword));
    return result;
}

char* OpenMFA::getHashedPin(){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    char *result = new char[strlen(OpenMFA_data.hashedPin)];
    memcpy(result, OpenMFA_data.hashedPin, strlen(OpenMFA_data.hashedPin));
    return result;
}



/*
 * OpenMFA get the Seed_Domain of a domain name in base 64
 */
char* OpenMFA::getDomainSeed(char* domain){
    char* seed = getSeed();
    
    char* result = hash(strcat(seed, domain));
    free(seed);
    return result;
}



/*
 * OpenMFA get the OTP of a domain name in base 64
 */
char* OpenMFA::getDomainOTP(char* domain, long timeInMS){
    char* domainSeed = getDomainSeed(domain);
    
    char time[50];
    sprintf(time, "%l", timeInMS);
    
    char* result = hash(strcat(domainSeed, time));
    free(domainSeed);
    return result;
}


//  To get the one time pin based on the pin and nonce
char* OpenMFA::getOneTimePin(char* pinNonce){
   char* pin = getHashedPin();
   
   char* result = hash(strcat(pin, pinNonce));
   free(pin);
   return result;
}



/*
 * Hash function
 */
char* OpenMFA::hash(char *s)
{
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(s);
    char *result = (char*) Sha1.resultHmac();
    
    //  encode to base64
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    char resultBase64[base64_length];
    base64_encode(resultBase64, result, SHA1_DEFAULT_KEY_LENGTH);
    free(result);
    return resultBase64;
    //char *output = new char[base64_length];
    //memcpy(output, resultBase64, base64_length);
    //return output;
}



/*
 * Xor function for base64 strings
 */
char* OpenMFA::xorBase64(char* msg, char* key)
{
    //  Convert to binary
    int base64_length_msg = strlen(msg);
    int binary_length_msg = base64_dec_len(msg, base64_length_msg);
    char *decodedMsg = new char[binary_length_msg];
    base64_decode(decodedMsg, (char*) Sha1.resultHmac(), base64_length_msg);
    
    int base64_length_key = strlen(key);
    int binary_length_key = base64_dec_len(key, base64_length_msg);
    char *decodedKey = new char[binary_length_key];
    base64_decode(decodedKey, (char*) Sha1.resultHmac(), base64_length_key);
    
    //  Actual XOR
    char *decodedResult = new char[binary_length_msg];
    for (int i = 0 ; i < binary_length_msg; i++)
      decodedResult[i] = decodedMsg[i] ^ decodedKey[i % binary_length_key];
      
    //  Encode result
    int base64_length = base64_enc_len(binary_length_msg);
    char *output = new char[base64_length];
    base64_encode(output, decodedResult, binary_length_msg);
    
    free(decodedMsg);
    free(decodedKey);
    free(decodedResult);
    return output;
}



/*
 * OpenMFA generate random 32 Bytes
 */
char* OpenMFA::generateRandomBytes(int bytes)
{
    char *bits = new char[bytes];
    for (int i = 0; i < bytes; i++)
    bits[i] = random(0, 256);
    return bits;
}


