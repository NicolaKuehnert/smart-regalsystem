#ifndef BLE_H
#define BLE_H

#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

/*
* Setze den Status, ob ein Gerät verbunden ist, oder nicht
*/
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

/*
* Events, die der Server mit dem Client verbinden
*/
class MyCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic *pCharacteristic);
};

class MyFileCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic *pCharacteristic);
};

class ble{
    public:
      int init();
      ble();
      bool connected();
    private:
      int disconnect();
};

#endif