package com.mfaandroid.app;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

/**
 * Created by khankuan on 20/1/14.
 */
public class MFAService extends Service {
    BluetoothAcceptThread bluetoothAcceptThread;
    final UUID sppUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    MFADevice mfaDevice;

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        mfaDevice = new MFADevice(this.getApplicationContext());
        if (mfaDevice.getUuid().length() == 0)
            mfaDevice.resetDevice();
        bluetoothAcceptThread = new BluetoothAcceptThread("MFA", sppUuid, MFAService.this);
        bluetoothAcceptThread.start();
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.d("MFAService", "Destroy");
        bluetoothAcceptThread.cancel();
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d("MFAService", "Bind");
        return null;
    }

    public void handleMessage(String msg, CallBack callback) {
        doNotify(5000);

        try {
            JSONObject m = new JSONObject(msg);
            JSONObject response = new JSONObject();

            String queryType = m.getString("queryType");
            String timestamp = m.getString("timestamp");


            if (queryType.equals("getUuid")){
                response.put("uuid", mfaDevice.getUuid());


            } else if (queryType.equals("getName")){
                response.put("name", mfaDevice.getName());


            } else if (queryType.equals("setPassword")){
                String password = m.getString("password");
                String newPassword = m.getString("newPassword");
                if (mfaDevice.setPassword(password, newPassword))
                    response.put("success", true);
                else
                    response.put("success", false);


            } else if (queryType.equals("setPin")){
                String password = m.getString("password");
                String newPin = m.getString("newPin");
                if (mfaDevice.setPin(password, newPin))
                    response.put("success", true);
                else
                    response.put("success", false);


            } else if (queryType.equals("setName")){
                String password = m.getString("password");
                String newName = m.getString("newName");
                if (mfaDevice.setName(password, newName))
                    response.put("success", true);
                else
                    response.put("success", false);


            } else if (queryType.equals("getDomainSeed_E_Pin")){
                String domain = m.getString("domain");
                String pinNonce = m.getString("pinNonce");
                response.put("domainSeed_E_Pin", mfaDevice.getDomainSeed_E_Pin(domain, pinNonce));
                response.put("domainSeedAnswer", mfaDevice.getDomainSeed(domain));
                response.put("uuid", mfaDevice.getUuid());
                response.put("name", mfaDevice.getName());


            } else if (queryType.equals("getDomainOTP_E_Pin")){
                String domain = m.getString("domain");
                String pinNonce = m.getString("pinNonce");
                response.put("domainOTP_E_Pin", mfaDevice.getDomainOTP_E_Pin(domain, pinNonce));
                response.put("domainOTPAnswer", mfaDevice.getDomainOTP(domain));
                response.put("uuid", mfaDevice.getUuid());
                response.put("name", mfaDevice.getName());


            } else if (queryType.equals("resetDevice")){
                mfaDevice.resetDevice();
                response.put("success", true);


            } else if (queryType.equals("updateInfo")){
                String password = m.getString("password");
                String newName = null, newPin = null, newPassword = null;
                try{
                    newName = m.getString("newName");
                } catch (Exception e){};
                try{
                    newPin = m.getString("newPin");
                } catch (Exception e){};
                try{
                    newPassword = m.getString("newPassword");
                } catch (Exception e){};
                boolean valid = true;

                if (newName != null)
                    valid = valid && mfaDevice.setName(password, newName);
                if (newPin != null)
                    valid = valid && mfaDevice.setPin(password, newPin);
                if (newPassword != null)
                    valid = valid && mfaDevice.setPassword(password, newPassword);

                if (valid)
                    response.put("success", true);
                else
                    response.put("success", false);

            } else if (queryType.equals("getInfo")){
                response.put("uuid", mfaDevice.getUuid());
                response.put("name", mfaDevice.getName());
                response.put("seed", mfaDevice.getSeed());
                response.put("hashedPassword", mfaDevice.getHashedPassword());
                response.put("hashedPin", mfaDevice.getHashedPin());
            }

            //  Response
            response.put("queryType", queryType);
            response.put("timestamp", timestamp);
            callback.onResult(response);

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }



    /*  Notification    */
    int LED_NOTIFICATION_ID = 8273;
    private Handler mHandler = new Handler();

    private Runnable mClearLED_Task = new Runnable(){
        public void run(){
            NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
            nm.cancel(LED_NOTIFICATION_ID);
            Log.d("MFAService", "Cancel Notification");
        }
    };

    public void doNotify(int duration){
        NotificationManager nm = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
        Notification notif = new Notification();
        notif.ledARGB = 0xFFffffff;
        notif.flags = Notification.FLAG_SHOW_LIGHTS;
        notif.ledOnMS = 500;
        notif.ledOffMS = 300;
        notif.flags |= Notification.FLAG_SHOW_LIGHTS;
        notif.priority = Notification.PRIORITY_MAX;
        nm.notify(LED_NOTIFICATION_ID, notif);
        Log.d("MFASerivce", "Do Notification");

        mHandler.postDelayed(mClearLED_Task, duration);
    };
}

interface CallBack{
    void onResult(JSONObject result);
}