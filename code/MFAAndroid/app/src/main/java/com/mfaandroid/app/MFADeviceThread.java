package com.mfaandroid.app;

import android.bluetooth.BluetoothSocket;
import android.util.Log;

import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by khankuan on 18/1/14.
 */
public class MFADeviceThread extends Thread implements CallBack {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private int delimiter = 10;
    private int timeout = 5000;
    Thread deathThread;
    MFAService mfaService;

    public MFADeviceThread(BluetoothSocket socket, MFAService mfaService) {

        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
        this.mfaService = mfaService;
        deathTimer();
        Log.d("MFADeviceThread", "Started");
    }

    public void run() {
        byte[] buffer = new byte[1024];  // buffer store for the stream
        int bytes; // bytes returned from read()

        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                // Read from the InputStream
                bytes = mmInStream.read(buffer);
                for (int i = 0; i < bytes; i++){
                    byte b = buffer[i];

                    if (b == delimiter){
                        byte[] encodedBytes = new byte[i];
                        System.arraycopy(buffer, 0, encodedBytes, 0, encodedBytes.length);
                        buffer = new byte[1024];  // buffer store for the stream

                        final String data = new String(encodedBytes, "UTF-8");
                        Log.d("MFADeviceThread-read", data);
                        mfaService.handleMessage(data, this);
                    }
                }
            } catch (IOException e) {
                Log.d("MFADeviceThread-read", e.toString());
                this.cancel();
                break;
            }
        }
    }

    /** Call this from the main activity to send data to the remote device */
    public void write(String msg) {
        try {
            Log.d("MFADeviceThread-write", msg);
            msg += (char)delimiter;
            byte[] bytes = msg.getBytes();
            mmOutStream.write(bytes);
        } catch (IOException e) {
            this.cancel();
            Log.d("MFADeviceThread-write", e.toString());
        }
    }

    /** Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
            this.interrupt();
            deathThread.interrupt();
            Log.d("MFADeviceThread", "Cancel");
        } catch(Exception e){
        }
    }

    /** Kill connection after x seconds */
    private void deathTimer(){
        deathThread = new Thread(){
            public void run() {
                try {
                    Thread.sleep(timeout);
                } catch (InterruptedException e) {
                }
                if (!MFADeviceThread.this.isInterrupted())
                    MFADeviceThread.this.cancel();
            }
        };
        deathThread.start();
        Log.d("MFADeviceThread", "Deathtimer");
    }

    @Override
    public void onResult(JSONObject result) {
        write(result.toString());
    }

    public boolean isConnected(){
        return mmSocket.isConnected();
    }
}