package com.mfaandroid.app;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Base64;
import android.util.Log;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Random;

/**
 * Created by khankuan on 20/1/14.
 */
public class MFADevice {
    /**  Private variables    */
    private SharedPreferences sharedPref;
    private SharedPreferences.Editor sharedPrefEditor;

    /**  Constructor */
    public MFADevice(Context context){
        this.sharedPref = context.getSharedPreferences(context.getString(R.string.app), Context.MODE_PRIVATE);
        this.sharedPrefEditor = this.sharedPref.edit();
    }

    /**  Public methods  */
    public String getUuid(){
        return sharedPref.getString("uuid", "");
    }

    public String getName(){
        return sharedPref.getString("name", "");
    }
    
    public boolean setPassword(String inputOldPassword, String inputNewPassword){
        if (inputNewPassword.length() == 0)
            return false;

        String oldPasswordHashed = sharedPref.getString("hashedPassword", "");
        String inputOldPasswordHashed = hash(inputOldPassword);
        String inputNewPasswordHashed = hash(inputNewPassword);

        //  Verify Password
        if (oldPasswordHashed.equals(inputOldPasswordHashed)){
            sharedPrefEditor.putString("hashedPassword", inputNewPasswordHashed);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public boolean setPin(String inputPassword, String inputNewPin){
        if (inputNewPin.length() == 0)
            return false;

        String oldPasswordHashed = sharedPref.getString("hashedPassword", "");
        String inputPasswordHashed = hash(inputPassword);
        String inputNewPinHashed = hash(inputNewPin);

        //  Verify Password
        if (oldPasswordHashed.equals(inputPasswordHashed)){
            sharedPrefEditor.putString("hashedPin", inputNewPinHashed);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public boolean setName(String inputPassword, String inputNewName){
        if (inputNewName.length() == 0)
            return false;

        String oldPasswordHashed = sharedPref.getString("hashedPassword", "");
        String inputPasswordHashed = hash(inputPassword);

        //  Verify Password
        if (oldPasswordHashed.equals(inputPasswordHashed)){
            sharedPrefEditor.putString("name", inputNewName);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public String getDomainSeed_E_Pin(String domain, String pinNonce){
        String domainSeed = getDomainSeed(domain);

        //  Get OneTimePin
        String oneTimePin = getOneTimePin(pinNonce);

        //  Encrypt SeedDomain based on XOR
        return xorBase64(domainSeed, oneTimePin);
    }

    public String getDomainOTP_E_Pin(String domain, String pinNonce){
        //  Get OTP
        String otp = getDomainOTP(domain);

        //  Get OneTimePin
        String oneTimePin = getOneTimePin(pinNonce);

        //  Encrypt OTP based on XOR
        return xorBase64(otp, oneTimePin);
    }

    public void resetDevice(){
        Random rand = new Random();
        rand.setSeed(System.nanoTime());
        long id = Math.abs(rand.nextLong() % 1000);
        String uuid = "UUID-"+id;
        String name = "Name-"+id;
        String seed = hash(rand.nextLong()+"");
        String hashedPassword = hash("default");
        String hashedPin = hash("default");

        sharedPrefEditor.putString("uuid", uuid);
        sharedPrefEditor.putString("name", name);
        sharedPrefEditor.putString("seed", seed);
        sharedPrefEditor.putString("hashedPassword", hashedPassword);
        sharedPrefEditor.putString("hashedPin", hashedPin);
        sharedPrefEditor.commit();
    }



    /**  Test methods */
    public String getSeed(){
        String seed = sharedPref.getString("seed", "");
        return seed;
    }

    public String getHashedPassword(){
        String hashedPassword = sharedPref.getString("hashedPassword", "");
        return hashedPassword;
    }

    public String getHashedPin(){
        String hashedPin = sharedPref.getString("hashedPin", "");
        return hashedPin;
    }



    /**  Private methods for reuse */
    public String getDomainSeed(String domain){
        String seed = getSeed();
        return hash(seed + domain);
    }

    public String getDomainOTP(String domain){
        String domainSeed = getDomainSeed(domain);

        //  Make OTP
        long timeStamp = System.currentTimeMillis()/1000/60/5;  //    Time in minutes
        return hash(domainSeed+timeStamp);
    }

    private String getOneTimePin(String nonce){
        String seed = sharedPref.getString("hashedPin", "");
        return hash(seed + nonce);
    }

    /*  Helper methods  */
    public static String hash(String s){
        MessageDigest digest;
        try {
            digest = MessageDigest.getInstance("SHA-1");
            digest.update(s.getBytes(),0,s.length());
            String h = new String(Base64.encode(digest.digest(), Base64.DEFAULT));
            return h.replace("\n", "");
        }
        catch (NoSuchAlgorithmException e){
            e.printStackTrace();
        }
        return "";
    }

    public static String xorBase64(String msg, String key){
        byte[] decodedMsg = Base64.decode(msg, Base64.DEFAULT);
        byte[] decodedKey = Base64.decode(key, Base64.DEFAULT);

        byte[] decodedResult = new byte[decodedMsg.length];
        for (int i = 0; i < decodedMsg.length; i++)
            decodedResult[i] = (byte) (decodedMsg[i] ^ decodedKey[i % key.length()]);
        String output = new String(Base64.encode(decodedResult, Base64.DEFAULT));
        return output.replace("\n", "");
    }
}
