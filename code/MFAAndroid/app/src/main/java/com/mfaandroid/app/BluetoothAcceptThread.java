package com.mfaandroid.app;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.util.UUID;

public class BluetoothAcceptThread extends Thread {

    /** Variables   */
    BluetoothAdapter mBluetoothAdapter;
    private BluetoothServerSocket mmServerSocket;
    MFAService mfaService;
    MFADeviceThread mfaDeviceThread;

    /** Constructor */
    public BluetoothAcceptThread(String name, UUID uuid, MFAService mfaService) {
        this.mfaService = mfaService;
        BluetoothServerSocket tmp = null;
        try {
            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            tmp = mBluetoothAdapter.listenUsingInsecureRfcommWithServiceRecord(name, uuid);
        } catch (Exception e) {
            Log.d("BluetoothAcceptThread", "Bluetooth failed");
        }
        mmServerSocket = tmp;
    }

    /** Accept Loop */
    public void run() {
        BluetoothSocket socket = null;
        if (mmServerSocket == null){
            cancel();
            return;
        }
        Log.d("BluetoothAcceptThread", "Listening");
        while (true) {
            try {
                socket = mmServerSocket.accept();
                Log.d("BluetoothAcceptThread", "Accepted");
            } catch (Exception e) {
                Log.d("BluetoothAcceptThread", e.toString());
                break;
            }
            // If a connection was accepted
            if (socket != null) {
                manageConnectedSocket(socket);
            }
        }
    }

    public void cancel(){
        try {
            mmServerSocket.close();
            mfaDeviceThread.cancel();
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.interrupt();
        this.mfaService.stopSelf();
    }


    /** Socket handling */
    void manageConnectedSocket(BluetoothSocket socket){
        if (mfaDeviceThread != null && !mfaDeviceThread.isInterrupted() && mfaDeviceThread.isConnected()){
            try {
                socket.close();
                Log.d("BluetoothAcceptThread", "Rejected");
            } catch (IOException e) {
                e.printStackTrace();
            }
            return;
        }
        mfaDeviceThread = new MFADeviceThread(socket, this.mfaService);
        mfaDeviceThread.start();
    }
}