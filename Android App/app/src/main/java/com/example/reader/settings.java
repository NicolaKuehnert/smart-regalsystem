package com.example.reader;

import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import android.content.Intent;
import android.os.Handler;
import android.view.View;

import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import android.widget.Switch;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.Set;

public class settings extends AppCompatActivity {

    private BluetoothAdapter BA;
    private Set<BluetoothDevice>pairedDevices;
    ListView lv;
    String DeviceName;
    String deviceHardwareAddress;
    ArrayList LeDevices;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.settings_activity);
        getSupportFragmentManager()
                .beginTransaction()
                .commit();
        ActionBar actionBar = getSupportActionBar();
        if (actionBar != null) {
            actionBar.setDisplayHomeAsUpEnabled(true);
        }

        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }


        BA = BluetoothAdapter.getDefaultAdapter();
        lv = findViewById(R.id.listView);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

                Object o = lv.getItemAtPosition(position);
                DeviceName = o.toString();

                Toast.makeText(getApplicationContext(),"Connecting to: " + DeviceName, Toast.LENGTH_SHORT).show();

                try{
                    if (pairedDevices.size() > 0) {
                        // There are paired devices. Get the name and address of each paired device.
                        for (BluetoothDevice device : pairedDevices) {
                            if(DeviceName == device.getName()){
                                deviceHardwareAddress = device.getAddress(); // MAC address
                            }
                        }
                    }
                } catch (NullPointerException e){

                }




            }
        }
        );

        if(BA.isEnabled()){
            Switch BluetoothSwitch = findViewById(R.id.switch1);
            BluetoothSwitch.setChecked(true);
            list();
        }
    }

    public void onSwitch(View v){
        Switch BluetoothSwitch = findViewById(R.id.switch1);

        if(BluetoothSwitch.isChecked()){
            on();
        } else {
            off();
        }
    }

    public void on(){
        if (!BA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on",Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Already on", Toast.LENGTH_LONG).show();
        }

        list();
    }

    public void off(){
        BA.disable();
        Toast.makeText(getApplicationContext(), "Turned off" ,Toast.LENGTH_LONG).show();
    }

    private boolean mScanning;
    private Handler handler = new Handler();

    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

    // Device scan callback.
    private BluetoothAdapter.LeScanCallback leScanCallback =
            new BluetoothAdapter.LeScanCallback() {
                @Override
                public void onLeScan(final BluetoothDevice device, int rssi,
                                     byte[] scanRecord) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            LeDevices.add(device);
                        }
                    });
                }
            };

    private void scanLeDevice(final boolean enable) {

        if (enable) {
            // Stops scanning after a pre-defined scan period.
            handler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    mScanning = false;
                    BA.stopLeScan(leScanCallback);
                }
            }, SCAN_PERIOD);

            mScanning = true;
            BA.startLeScan(leScanCallback);
        } else {
            mScanning = false;
            BA.stopLeScan(leScanCallback);
        }
    }

    public void list(){

            scanLeDevice(true);

            if(LeDevices == null){
                Toast.makeText(getApplicationContext(), "No BLE Devices found!", Toast.LENGTH_LONG).show();
                return;
            }
            final ArrayAdapter adapter = new  ArrayAdapter(this,android.R.layout.simple_list_item_1, LeDevices);

            lv.setAdapter(adapter);


    }
}