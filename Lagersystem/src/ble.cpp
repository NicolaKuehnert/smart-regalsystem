#include "ble.h"
#include "scan.h"
BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic_search = NULL;
BLECharacteristic* pCharacteristic_file = NULL;
bool deviceConnected = false;
uint16_t conDeviceUUID;


/*
* Die Services und Charakteristiken brauchen unikate IDs
*/
#define SERVICE_UUID                "6d350151-ab8d-4ab6-9258-078ebda04511"
#define CHARACTERISTIC_UUID_SEARCH  "6d350151-ab8d-4ab6-9258-078ebda04501"
#define CHARACTERISTIC_UUID_FILE    "6d350151-ab8d-4ab6-9258-078ebda04502"

void MyServerCallbacks::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    conDeviceUUID = pServer->getConnId();
    M5.Lcd.fillRect(0,140,320,50,0);
    M5.Lcd.setCursor(70,150);
    M5.Lcd.println("Connected");
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    pServer->removePeerDevice(conDeviceUUID,pServer);
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

    std::string value = pCharacteristic->getValue();
    M5.Lcd.setCursor(0,100);
    searchTag(filename, value);
  }

  /*
* Client fragt Daten an / Server schreibt Daten
*/
void MyFileCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    M5.Lcd.clearDisplay();
    M5.Lcd.println("read");
    File file = SD.open(filename);
    char text = file.read();
    double data = (double)text;
    pCharacteristic->setValue(data);
    
    

  }
  
/*
* Server empfängt Daten / Client schreibt Daten
*/
void MyFileCallbacks::onWrite(BLECharacteristic *pCharacteristic) {

    std::string value = pCharacteristic->getValue();
    M5.Lcd.setCursor(0,100);
    searchTag(filename, value);
  }

ble::ble(){
    
}

/*
* Initialisiere BLE und starte den GATT Server
* Erstelle einen Service mit einer Charakteristik, die das verbundene Gerät abhören kann
* Werde für andere Geräte sichtbar
*/
int ble::init(){

    BLEDevice::init("m5-stack");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic_search = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID_SEARCH,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_NOTIFY |
                                    BLECharacteristic::PROPERTY_INDICATE
                                );
    pCharacteristic_search->setCallbacks(new MyCallbacks());
    pCharacteristic_search->addDescriptor(new BLE2902());
    pCharacteristic_file = pService->createCharacteristic(
                                    CHARACTERISTIC_UUID_FILE,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_NOTIFY |
                                    BLECharacteristic::PROPERTY_INDICATE
                                );
    pCharacteristic_file->setCallbacks(new MyFileCallbacks());
    pCharacteristic_file->addDescriptor(new BLE2902());
    pService->start();

    pServer->startAdvertising();

    return 0;

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
