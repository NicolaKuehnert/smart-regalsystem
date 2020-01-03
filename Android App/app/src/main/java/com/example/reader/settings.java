package com.example.reader;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Switch;
import android.widget.Toast;
import java.util.ArrayList;

public class settings extends AppCompatActivity {

    ListView lv;
    ArrayList LeDevices;
    private static final String TAG = "BLE_READER";
    BLEConnection bleConnection;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        /*
         * Pre-created constructor to initialize settings view
         */
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
        bleConnection = new BLEConnection();
        final Context con = this;

        /*
         * Create event if item in ListView is clicked
         * Set information for bleConnection
         * Connects to selected item from ListView
         */
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {

                Object o = lv.getItemAtPosition(position);
                bleConnection.DeviceName = o.toString();

                Toast.makeText(getApplicationContext(),"Connecting to: " + bleConnection.DeviceName, Toast.LENGTH_SHORT).show();

                try{
                    if (bleConnection.scannedDevices.size() > 0) {
                        for (BluetoothDevice dev : bleConnection.scannedDevices) {
                            if(bleConnection.DeviceName.compareTo(dev.getName()) == 0){
                                bleConnection.device = dev;
                                bleConnection.deviceHardwareAddress = bleConnection.device.getAddress(); // MAC address


                                int status = bleConnection.connect(con);
                                if(status != 0){
                                    Log.i(TAG, "Error connecting");
                                }

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

    /*
     * Searches bleConnection for bonded devices
     * Calls list()
     */
    public void search(View view){
        bleConnection.search();
        list();
    }

    /*
     * Toggles bluetooth on and off
     * Calls on() and off()
     */
    public void onSwitch(View v){
        Switch BluetoothSwitch = findViewById(R.id.switch1);

        if(BluetoothSwitch.isChecked()){
            on();
        } else {
            off();
        }
    }

    /*
     * If not on, turns Bluetooth on
     */
    private void on(){
        if (!bleConnection.BA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Toast.makeText(getApplicationContext(), "Turned on",Toast.LENGTH_LONG).show();
        } else {
            Toast.makeText(getApplicationContext(), "Already on", Toast.LENGTH_LONG).show();
        }

    }

    /*
     * Turns off Bluetooth
     */
    private void off(){
        bleConnection.BA.disable();
        Toast.makeText(getApplicationContext(), "Turned off" ,Toast.LENGTH_LONG).show();
    }

    /*
     * Lists all bonded devices in ListView
     */
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

    /*
     * Disconnects bleConnection
     */
    public void disconnect(View v){
        bleConnection.disconnect();

    }

    /*
     * If a connection is established, calls bleConnection.sendUpdate to send a message to the bonded device
     */
    public void update(View v){
        if(BLEConnection.pubBLE != null){
            bleConnection.sendUpdate("Hello World!");
        }
        else {
            Toast.makeText(this, "No BLE", Toast.LENGTH_SHORT).show();
        }

    }

    public void backClick(View v){

        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        finish();
    }

}