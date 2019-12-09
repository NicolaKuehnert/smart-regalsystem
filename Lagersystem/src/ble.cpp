#include "ble.h"

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
uint16_t conDeviceUUID;

/*
* Die Services und Charakteristiken brauchen unikate IDs
*/
#define SERVICE_UUID        "6d350151-ab8d-4ab6-9258-078ebda04511"
#define CHARACTERISTIC_UUID "6d350151-ab8d-4ab6-9258-078ebda04501"

void MyServerCallbacks::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    conDeviceUUID = pServer->getConnId();
    M5.Lcd.fillRect(0,140,320,50,0);
    M5.Lcd.setCursor(70,150);
    M5.Lcd.println("Connected");
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    M5.Lcd.fillRect(0 ,140, 320, 50 ,0);
    M5.Lcd.setCursor(70,150);
    M5.Lcd.println("Disconnected");

    
}

/*
* Client fragt Daten an / Server schreibt Daten
*/
void MyCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    M5.Lcd.clearDisplay();
    M5.Lcd.println("read");
    pCharacteristic->setValue("Hello World!");
  }
  
/*
* Server empfängt Daten / Client schreibt Daten
*/
void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    M5.Lcd.clearDisplay();
    M5.Lcd.println("write");
    std::string value = pCharacteristic->getValue();
    M5.Lcd.setCursor(10,10);
    M5.Lcd.println(value.c_str());
  }


/*
* Initialisiere BLE und starte den GATT Server
* Erstelle einen Service mit einer Charakteristik, die das verbundene Gerät abhören kann
*/
ble::ble(){
    BLEDevice::init("m5-stack");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_NOTIFY |
                                    BLECharacteristic::PROPERTY_INDICATE
                                );
    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

/*
* Schließe die Verbindung zum verbundenen Gerät und werde unsichtbar
*/
int ble::disconnect(){
    pServer->getAdvertising()->stop();
    pServer->disconnect(conDeviceUUID);
    return 0;
}

bool ble::connected(){
    return deviceConnected;
}
