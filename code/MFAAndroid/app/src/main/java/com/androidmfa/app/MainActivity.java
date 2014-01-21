package com.androidmfa.app;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ToggleButton;


public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.container, new PlaceholderFragment())
                    .commit();
        }
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;
        }
    }

    @Override
    public void onStart(){
        super.onStart();
        new CheckServiceThread().start();
    }

    public void toggle(View v){
        ToggleButton b = (ToggleButton) v;
        Intent intent = new Intent(this, MFAService.class);

        if (b.isChecked())
            startService(intent);
        else
            stopService(intent);
        Log.d("MainActivity", isMyServiceRunning()+"");
    }


    private boolean isMyServiceRunning() {
        ActivityManager manager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
        for (ActivityManager.RunningServiceInfo service : manager.getRunningServices(Integer.MAX_VALUE)) {
            if (MFAService.class.getName().equals(service.service.getClassName())) {
                return true;
            }
        }
        return false;
    }

    private void checkToggleButton(){
        runOnUiThread(new Runnable(){
            public void run(){
            ToggleButton s = (ToggleButton) findViewById(R.id.switch1);
            if (s == null)
                return;
            if (isMyServiceRunning())
                s.setChecked(true);
            else
                s.setChecked(false);
            }
        });
    }

    public class CheckServiceThread extends Thread{
        public void run(){
            while (true){
                checkToggleButton();
                try {
                    Thread.sleep(5000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    break;
                }
            }
        }
    }
}



