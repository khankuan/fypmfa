/*
This module contains the variables and methods required for the OpenMFA device.

seed, hashed_password and hashed_pin are all results from HMAC_SHA1


*/

#include "EEPROMAnything.h"
#include "Sha1.h"

#ifndef OpenMFA_h
#define OpenMFA_h

#define SHA1_DEFAULT_KEY "QZfd31IAlm1pMlnmbVCO"
#define SHA1_DEFAULT_KEY_LENGTH 20

// OpenMFA_Device related data
typedef struct
{
    char uuid[32];                /* uuid of the device */
    char name[32];                /* readable name of the device */
    char seed[32];                /* seed of device */

    char hashedPassword[32];     /* SHA-1 hashed password of device */
    char hashedPin[32];          /* SHA-1 hashed pin of device */
}
OpenMFA_data;


// OpenMFA_Device related functions
class OpenMFA
{
public:
    OpenMFA();
    char* getUuid();
    char* getName();
    bool setPassword(char* inputOldPassword, char* inputNewPassword);
    bool setPin(char* inputPassword, char* inputNewPin);
    bool setName(char* inputPassword, char* inputNewName);
    
    char* getDomainSeed_E_Pin(char* domain, char* pinNonce);
    char* getDomainOTP_E_Pin(char* domain, char* pinNonce, long timeInMS);
    char* getDomainSeed(char* domain);
    char* getDomainOTP(char* domain, long timeInMS);
    
    void resetDevice();


    //  Test methods
    char* getSeed();
    char* getHashedPassword();
    char* getHashedPin();
    
private:
    char* getOneTimePin(char* pinNonce);
    
    char* hash(char* s);
    char* xorBase64(char* msg, char* key);
    char* generateRandomBytes(int bytes);
};

#endif


