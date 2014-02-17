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
    char seed64[32];                /* seed of device */

    char hashed_password64[32];     /* SHA-1 hashed password of device */
    char hashed_pin64[32];          /* SHA-1 hashed pin of device */
}
OpenMFA_data;


// OpenMFA_Device related functions
class OpenMFA
{
public:
    OpenMFA();
    char* getUuid();
    char* getName();
    bool setPassword(char* old_password, char* new_password);
    bool setPin(char* password, char* new_pin);
    bool setName(char* password, char* new_name);
    
    char* getSeedDomain64_E_Pin(char* domain, long pin_nonce);
    char* getOTP64_E_Pin(char* domain, long time_in_ms, long pin_nonce);
    
    void resetDevice();


    //  Test methods
    char* getSeed64();
    char* getHashedPassword64();
    char* getHashedPin64();
    
private:
    char* getSeedDomain64(char* domain);
    char* generateRandomBytes(int bytes);
    char* getOneTimePin64(long nonce);
};

#endif


