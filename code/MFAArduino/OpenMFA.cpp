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
 * OpenMFA reset
 *
 */
void OpenMFA::resetDevice()
{
    OpenMFA_data OpenMFA_data;
    
    //  Memset
    memset (OpenMFA_data.uuid,0,32);
    memset (OpenMFA_data.name,0,32);
    memset (OpenMFA_data.seed64,0,32);
    memset (OpenMFA_data.hashed_password64,0,32);
    memset (OpenMFA_data.hashed_pin64,0,32);

    //  Flash data
    randomSeed(analogRead(0));
    unsigned long num = random(0, 100000000);
    unsigned long numName = num % 1000;
    
    ("UUID-" + String(num)+"\0").toCharArray(OpenMFA_data.uuid, 32);
    ("Name-" + String(numName)+"\0").toCharArray(OpenMFA_data.name, 32);
    
    //  Seed
    char *randomBytes = generateRandomBytes(32);
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(randomBytes);
    base64_encode(OpenMFA_data.seed64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    free(randomBytes);
    
    //  Password and pin
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print("default");
    base64_encode(OpenMFA_data.hashed_password64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    strcpy(OpenMFA_data.hashed_pin64, OpenMFA_data.hashed_password64);  //  pin same as password for default
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
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


/*
 * OpenMFA set password of this device
 */
bool OpenMFA::setPassword(char* old_password, char* new_password)
{
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    //  Hash input password
    char* input_hashed_password;
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(old_password);
    input_hashed_password = (char*) Sha1.resultHmac();
    
    //  Check if old password == current password
    char input_hashed_password_64[base64_enc_len(SHA1_DEFAULT_KEY_LENGTH)+1];
    base64_encode(input_hashed_password_64, input_hashed_password, SHA1_DEFAULT_KEY_LENGTH);
    if (memcmp(input_hashed_password_64, OpenMFA_data.hashed_password64, SHA1_DEFAULT_KEY_LENGTH) != 0)
        return false;
        
    //  Set new password
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(new_password);
    base64_encode(OpenMFA_data.hashed_password64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
    return true;
}



/*
 * OpenMFA set pin of this device
 */
bool OpenMFA::setPin(char* password, char* new_pin)
{
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    //  Hash input password
    char* input_hashed_password;
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(password);
    input_hashed_password = (char*) Sha1.resultHmac();
    
    //  Check old password == current password
    char input_hashed_password_64[base64_enc_len(SHA1_DEFAULT_KEY_LENGTH)+1];
    base64_encode(input_hashed_password_64, input_hashed_password, SHA1_DEFAULT_KEY_LENGTH);  
    if (memcmp(input_hashed_password_64, OpenMFA_data.hashed_password64, SHA1_DEFAULT_KEY_LENGTH) != 0)
        return false;
        
    //  Set new pin
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(new_pin);
    base64_encode(OpenMFA_data.hashed_pin64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    
    //  Store
    EEPROM_writeAnything(0, OpenMFA_data);
    return true;
}



/*
 * OpenMFA set name of this device
 */
bool OpenMFA::setName(char* password, char* new_name)
{
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);

    //  Hash input password
    char* input_hashed_password;
    Sha1.initHmac((uint8_t*)SHA1_DEFAULT_KEY, SHA1_DEFAULT_KEY_LENGTH);
    Sha1.print(password);
    input_hashed_password = (char*) Sha1.resultHmac();
    
    //  Check old password == current password
    char input_hashed_password_64[base64_enc_len(SHA1_DEFAULT_KEY_LENGTH)+1];
    base64_encode(input_hashed_password_64, input_hashed_password, SHA1_DEFAULT_KEY_LENGTH);
    if (memcmp(input_hashed_password_64, OpenMFA_data.hashed_password64, SHA1_DEFAULT_KEY_LENGTH) != 0)
        return false;
    
    //  Store
    String(new_name).toCharArray(OpenMFA_data.name, 32);
    EEPROM_writeAnything(0, OpenMFA_data);
    return true;
}



/*
 * OpenMFA get the Seed_Domain of a domain name, in base 64 and encrypted with pin
 */
char* OpenMFA::getSeedDomain64_E_Pin(char* domain, long pin_nonce)
{
    char* seed_domain64 = getSeedDomain64(domain);
    
    //  obtain h(pin, pin_none)
    char *one_time_pin = getOneTimePin64(pin_nonce);
    
    //  Xor to encrypt
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    char *cipher = new char[base64_length];
    for (int i = 0 ; i < base64_length; i++)
      cipher[i] = one_time_pin[i] ^ seed_domain64[i];
    
    free(seed_domain64);
    free(one_time_pin);
    return cipher;
}


/*
 * OpenMFA get the Seed_Domain of a domain name in base 64
 */
char* OpenMFA::getSeedDomain64(char* domain){
    //  Retrieve device data
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    //  Make seed_domain
    Sha1.initHmac((uint8_t*)OpenMFA_data.seed64, base64_enc_len(SHA1_DEFAULT_KEY_LENGTH));
    Sha1.print(domain);
    
    //  base 64
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    char *seed_domain64 = new char[base64_length];
    base64_encode(seed_domain64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    
    return seed_domain64;
}



/*
 * OpenMFA get the OTP of a domain name in base 64
 */
char* OpenMFA::getOTP64_E_Pin(char* domain, long time_in_minutes, long pin_nonce){
    char *seed_domain64 = getSeedDomain64(domain);
    
     //  Make OTP64
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    Sha1.initHmac((uint8_t*)seed_domain64, base64_length);
    Sha1.print(time_in_minutes);
    char OTP64[base64_length];
    base64_encode(OTP64, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    //Serial.println(OTP64);
    
     //  obtain h(pin, pin_none)
    char *one_time_pin = getOneTimePin64(pin_nonce);
    //Serial.println(one_time_pin);
    
    //  Xor to encrypt
    char *cipher = new char[base64_length];
    for (int i = 0 ; i < base64_length; i++)
      cipher[i] = one_time_pin[i] ^ OTP64[i];
    
    free(seed_domain64);
    free(OTP64);
    free(one_time_pin);
    return cipher;
}


//  To get the one time pin based on the pin and nonce
char* OpenMFA::getOneTimePin64(long pin_nonce){
   //  obtain h(pin, pin_none)
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    
    Sha1.initHmac((uint8_t*)OpenMFA_data.hashed_pin64, base64_enc_len(SHA1_DEFAULT_KEY_LENGTH));
    Sha1.print(pin_nonce);
    
    //  encode to base64
    int base64_length = base64_enc_len(SHA1_DEFAULT_KEY_LENGTH);
    char *one_time_pin = new char[base64_length];
    base64_encode(one_time_pin, (char*) Sha1.resultHmac(), SHA1_DEFAULT_KEY_LENGTH);
    
    return one_time_pin;
}

/*
 *
 *
 *  Test methods
 *
 *
 */


/*
 * OpenMFA get name
 */
 
char* OpenMFA::getSeed64()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[32];
    strcpy(output, OpenMFA_data.seed64);
    return output;
}
/*
 * OpenMFA get name
 */
char* OpenMFA::getHashedPassword64()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[32];
    strcpy(output, OpenMFA_data.hashed_password64);
    return output;
}

/*
 * OpenMFA get name
 */      
char* OpenMFA::getHashedPin64()
{
    OpenMFA_data OpenMFA_data;
    EEPROM_readAnything(0, OpenMFA_data);
    char *output = new char[32];
    strcpy(output, OpenMFA_data.hashed_pin64);
    return output;
}



