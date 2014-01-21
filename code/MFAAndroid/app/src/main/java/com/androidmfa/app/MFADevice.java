package com.androidmfa.app;

import android.content.Context;
import android.content.SharedPreferences;

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
        return sharedPref.getString("Uuid", "");
    }

    public String getName(){
        return sharedPref.getString("Name", "");
    }
    
    public boolean setPassword(String inputOldPassword, String inputNewPassword){
        String oldPasswordHashed = sharedPref.getString("HashedPassword", "");
        String inputOldPasswordHashed = hash(inputOldPassword);
        String inputNewPasswordHashed = hash(inputNewPassword);

        //  Verify Password
        if (oldPasswordHashed.equals(inputOldPasswordHashed)){
            sharedPrefEditor.putString("HashedPassword", inputNewPasswordHashed);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public boolean setPin(String inputPassword, String inputNewPin){
        String oldPasswordHashed = sharedPref.getString("HashedPassword", "");
        String inputPasswordHashed = hash(inputPassword);
        String inputNewPinHashed = hash(inputNewPin);

        //  Verify Password
        if (oldPasswordHashed.equals(inputPasswordHashed)){
            sharedPrefEditor.putString("HashedPin", inputNewPinHashed);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public boolean setName(String inputPassword, String inputNewName){
        String oldPasswordHashed = sharedPref.getString("HashedPassword", "");
        String inputPasswordHashed = hash(inputPassword);

        //  Verify Password
        if (oldPasswordHashed.equals(inputPasswordHashed)){
            sharedPrefEditor.putString("Name", inputNewName);
            sharedPrefEditor.commit();
            return true;
        } else
            return false;
    }

    public String getSeedDomain_E_Pin(String domain, String pinNonce){
        String seedDomain = getSeedDomain(domain);

        //  Get OneTimePin
        String oneTimePin = getOneTimePin(pinNonce);

        //  Encrypt SeedDomain based on XOR
        return xor(seedDomain, oneTimePin);
    }

    public String getOTP_E_Pin(String domain, String pinNonce){
        //  Get OTP
        String otp = getOTP(domain);

        //  Get OneTimePin
        String oneTimePin = getOneTimePin(pinNonce);

        //  Encrypt OTP based on XOR
        return xor(otp, oneTimePin);
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

        sharedPrefEditor.putString("Uuid", uuid);
        sharedPrefEditor.putString("Name", name);
        sharedPrefEditor.putString("Seed", seed);
        sharedPrefEditor.putString("HashedPassword", hashedPassword);
        sharedPrefEditor.putString("HashedPin", hashedPin);
        sharedPrefEditor.commit();
    }



    /**  Test methods */
    public String getSeed(){
        String seed = sharedPref.getString("Seed", "");
        return seed;
    }

    public String getHashedPassword(){
        String hashedPassword = sharedPref.getString("HashedPassword", "");
        return hashedPassword;
    }

    public String getHashedPin(){
        String hashedPin = sharedPref.getString("HashedPin", "");
        return hashedPin;
    }



    /**  Private methods for reuse */
    public String getSeedDomain(String domain){
        String seed = sharedPref.getString("Seed", "");
        return hash(seed + domain);
    }

    public String getOTP(String domain){
        String seedDomain = getSeedDomain(domain);

        //  Make OTP
        long timeStamp = System.currentTimeMillis()/1000/60;  //    Time in minutes
        return hash(seedDomain+timeStamp);
    }

    private String getOneTimePin(String nonce){
        String seed = sharedPref.getString("HashedPin", "");
        return hash(seed + nonce);
    }

    /*  Helper methods  */
    public static String hash(String s){
        MessageDigest digest;
        try {
            digest = MessageDigest.getInstance("SHA-1");
            digest.update(s.getBytes(),0,s.length());
            String hash = new BigInteger(1, digest.digest()).toString(16);
            return hash;
        }
        catch (NoSuchAlgorithmException e)
        {
            e.printStackTrace();
        }
        return "";
    }

    public static String xor(String msg, String key){
        StringBuilder output = new StringBuilder();
        for (int i = 0; i < msg.length(); i++)
            output.append((char)(msg.charAt(i) ^ key.charAt(i % key.length())));
        return output.toString();
    }
}
