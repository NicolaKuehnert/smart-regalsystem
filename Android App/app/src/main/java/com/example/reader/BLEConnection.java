package com.example.reader;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.util.Log;

import java.util.List;
import java.util.Set;
import java.util.UUID;

public class BLEConnection {

    /*
     * Static active BLEConnection for use over all Android Views
     */
    public static BLEConnection pubBLE;

    /*
     * Protected:
     * Infos about the connected device
     *
     * Private:
     * active Gatt connection that should not be used outside of this class
     */
    protected BluetoothAdapter BA;
    protected Set<BluetoothDevice> scannedDevices;
    protected BluetoothDevice device;
    protected String DeviceName;
    protected String deviceHardwareAddress;
    private BluetoothGatt bluetoothGatt;
    private int reconnect = 0;

    //Tag used for logging
    private static final String TAG = "BLE_READER";

    /*
     * Constructor sets pubBLE if not set
     * Fetches default bluetooth adapter
     */
    public BLEConnection(){
        if (pubBLE == null){
            BA = BluetoothAdapter.getDefaultAdapter();
            pubBLE = this;
        }
    }


    /*
     * UUIDs for communication with M5Stack
     * Static strings used for logging
     */
    private final String CHARACTERISTIC_UUID = "6d350151-ab8d-4ab6-9258-078ebda04501";
    private final String SERVICE_UUID = "6d350151-ab8d-4ab6-9258-078ebda04511";

    public final static String ACTION_GATT_CONNECTED = "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED = "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED = "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE = "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA = "com.example.bluetooth.le.EXTRA_DATA";

    /*
     * Get all bonded devices
     */
    public void search(){
        scannedDevices = BA.getBondedDevices();
    }

    /*
     * Connect to set device
     * If connected, disconnect and reconnect
     * Starts service discovery
     */
    public int connect(Context context){

        if(bluetoothGatt != null){
            disconnect();
        }
        if(device == null){
            return 1;
        }

        bluetoothGatt = device.connectGatt(context, true, gattCallback);
        pubBLE = this;

        bluetoothGatt.discoverServices();
        return 0;


    }

    /*
     * Disconnect GATT from device
     */
    public void disconnect(){
        if (bluetoothGatt == null) {
            return;
        }
        bluetoothGatt.disconnect();
        bluetoothGatt = null;
        pubBLE = null;
        reconnect = 5;

    }

    /*
     * Update the characteristic and write message to the server
     */
    public void sendUpdate(String message){
        List<BluetoothGattService> gattServices = bluetoothGatt.getServices();
        for(BluetoothGattService item : gattServices){
            UUID itemId = item.getUuid();
            UUID clientId = UUID.fromString(CHARACTERISTIC_UUID);
            if (itemId.equals(clientId)){
                broadcastUpdate("Client Service UUID equals Item Service UUID");
                BluetoothGattCharacteristic chara = item.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID));
                chara.setValue(message);
                bluetoothGatt.writeCharacteristic(chara);
                broadcastUpdate("Update send: " + message);
            }
        }




    }

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        // Result of a characteristic read operation
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status){
            if (status == BluetoothGatt.GATT_SUCCESS){
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
            }

        }

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState){

            if(newState == BluetoothGatt.STATE_CONNECTED){
                String text = "Status: " + status + " | GATT CONNECTED";
                broadcastUpdate(text);
                gatt.discoverServices();
                reconnect = 0;
            }
            if(newState == BluetoothGatt.STATE_DISCONNECTED){
                broadcastUpdate("GATT DISCONNECTED");
                if(reconnect < 5){
                    broadcastUpdate("Trying to reconnect");
                    gatt.connect();
                    reconnect++;
                }
            }


        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            List<BluetoothGattService> gattServices = gatt.getServices();
            for(BluetoothGattService item : gattServices){
                UUID itemId = item.getUuid();
                UUID clientId = UUID.fromString(SERVICE_UUID);
                if (itemId.equals(clientId)){
                    broadcastUpdate("Client Service UUID equals Item Service UUID");
                    BluetoothGattCharacteristic chara = item.getCharacteristic(UUID.fromString(CHARACTERISTIC_UUID));
                    chara.setValue("Update!");
                    gatt.writeCharacteristic(chara);
                }
            }
        }
    };

    private void broadcastUpdate(final String action, final BluetoothGattCharacteristic characteristic){
        String value = characteristic.getValue().toString();
        Log.i(TAG, action + " : " + value);

    }

    private void broadcastUpdate(final String action){
        Log.i(TAG, action);
    }

}
