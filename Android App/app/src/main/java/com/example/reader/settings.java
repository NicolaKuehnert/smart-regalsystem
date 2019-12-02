package com.example.reader;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;

import android.content.Intent;
import android.view.View;

import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

public class settings extends AppCompatActivity {

    ListView lv;
    ArrayList LeDevices;


    BLEConnection bleConnection;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.settings_activity);
        getSupportFragmentManager()
                .beginTransaction()
                .commit();


        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }


        lv = findViewById(R.id.listView);
        bleConnection = new BLEConnection(this);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

                Object o = lv.getItemAtPosition(position);
                bleConnection.DeviceName = o.toString();

                Toast.makeText(getApplicationContext(),"Connecting to: " + bleConnection.DeviceName, Toast.LENGTH_SHORT).show();

                try{
                    if (bleConnection.scannedDevices.size() > 0) {
                        // There are paired devices. Get the name and address of each paired device.
                        for (BluetoothDevice dev : bleConnection.scannedDevices) {
                            if(bleConnection.DeviceName.compareTo(dev.getName()) == 0){
                                bleConnection.device = dev;
                                bleConnection.deviceHardwareAddress = bleConnection.device.getAddress(); // MAC address


                                bleConnection.connect();
                            }

                        }
                    }
                } catch (NullPointerException e){

                }
            }
        }
        );

        if(bleConnection.BA.isEnabled()){
            Switch BluetoothSwitch = findViewById(R.id.switch1);
            BluetoothSwitch.setChecked(true);
        }
    }

    public void search(View view){
        bleConnection.search();
        list();
    }

    public void onSwitch(View v){
        Switch BluetoothSwitch = findViewById(R.id.switch1);

        if(BluetoothSwitch.isChecked()){
            on();
        } else {
            off();
        }
    }

    private void on(){
        if (!bleConnection.BA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on",Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Already on", Toast.LENGTH_LONG).show();
        }

    }

    private void off(){
        bleConnection.BA.disable();
        Toast.makeText(getApplicationContext(), "Turned off" ,Toast.LENGTH_LONG).show();
    }

    private void list(){

            LeDevices = new ArrayList<String>();
            if(bleConnection.scannedDevices == null) {
                Toast.makeText(getApplicationContext(), "No BLE Devices found!", Toast.LENGTH_LONG).show();
                return;
            }

            for(BluetoothDevice device : bleConnection.scannedDevices){
                LeDevices.add(device.getName());
            }
            final ArrayAdapter adapter = new  ArrayAdapter(this,android.R.layout.simple_list_item_1, LeDevices);

            lv.setAdapter(adapter);


    }

    public void disconnect(View v){
        bleConnection.disconnect();

    }

}