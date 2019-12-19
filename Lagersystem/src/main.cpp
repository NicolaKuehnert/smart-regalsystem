#include <Arduino.h>
#include <M5Stack.h>
#include <SPI.h>
#include <MFRC522.h>

#include "printDisplay.h"
#include "search.h"
#include "ble.h"
//#include "MFRC522_I2C.h"
#include "ArduinoJson.h"


State state = scan;
uint32_t targetTime = 0;
int i_init = 0;
// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
//MFRC522 mfrc522(0x28);   // Create MFRC522 instance.
#define sleeptime 1
const char *tokensFile = "/datenbank.txt";

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);
/*
*Gibt das Menüfeld der Searchfunktion aus.
*Diese Methode wird ausgeführt wen der "A-Knopf" gedrückt wird und der Zustand >nicht< auf "search" ist.
*/
void printSearch(){
  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(PINK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(110, 50);
  M5.Lcd.println("SEARCH");
  if (connected == false)
  {
    M5.Lcd.setCursor(30, 100);
    M5.Lcd.println("nicht verbunden");
  } else
  {
     M5.Lcd.setCursor(100, 100);
    M5.Lcd.println("verbunden");
  }
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("Search");
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.println("Scan");
  
}
/*
*Gibt das Menüfeld der Scanfunktion aus.
*Diese Methode wird ausgeführt wen der "B-Knopf" gedrückt wird und der Zustand >nicht< auf "scan" ist.
*/
void printScan(){
  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(110, 50);
  M5.Lcd.println("SCAN");
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.setTextSize(2);
  M5.Lcd.println("Halte den mittleren Knopf gedrueckt um zu scannen");
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.println("Scan");
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("Search");
  
}

int check(ble context){
  int i = 0;
  while(i <= 100)
    {
      if(context.connected()){
        connected = true;
        printSearch();
        break;
      } else {
        
        M5.Lcd.progressBar(100, 100, 100, 10, i);
        
        i = i+10;

        sleep(sleeptime);
        
      }
    }
    if(context.connected()){
      connected = true;
    }
    printSearch();
    return 0;
}

/***********************************************
        FUNKTIONEN FÜR DIE SD KARTE
***********************************************/
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    
  Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
            M5.Lcd.print("  FILE: ");
            M5.Lcd.print(file.name());
            M5.Lcd.print("  SIZE: ");
            M5.Lcd.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path) {
    Serial.printf("Reading file: %s\n", path);
    String s;
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        int ch = file.read();
        s = file.parseInt();
        Serial.write(ch);
        M5.Lcd.print(s);
    }
    M5.Lcd.println(s);
    delay(10000);
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
}

/***********************************************
        FUNKTIONEN FÜR JSON MANIPULATION
***********************************************/


// Loads the configuration from a file
void loadConfiguration(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<512> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

}

// Saves the configuration to a file
void saveConfiguration(const char *filename) {
  // Delete existing file, otherwise the configuration is appended to the file
  SD.remove(filename);

  // Open file for writing
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/assistant to compute the capacity.
  StaticJsonDocument<256> doc;

  // Set the values in the document
 /* doc["hostname"] = config.hostname;
  doc["port"] = config.port;*/

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    M5.Lcd.print(buffer[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    M5.Lcd.print(buffer[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(buffer[i], DEC);
  }
}

void setup() {
  M5.begin(true, true, true);
  M5.Power.begin();
  Wire.begin();

  writeFile(SD, tokensFile, "{ \"96 28 57 F7\": true, \"66 20 6C F7\": 3.141 }");

 
  mfrc522.PCD_Init(); // Init MFRC522 

 

  /*char input[] = "{\"96 28 57 F7\":[48.756080,2.302038],\"66 20 6C F7\":[48.756080,2.302038]}";
  StaticJsonDocument<250> doc;
  DeserializationError error = deserializeJson(doc, input);

  // Test if parsing succeeds.
  if (error) {
    M5.Lcd.println(F("deserializeJson() failed: "));
    M5.Lcd.println(error.c_str());
    return;
  }
  const char* sensor = doc["failure"];
  double latitude = doc["66 20 6C F7"][0];
  double longitude = doc["66 20 6C F7"][1];*/

  // Print values.
 /* M5.Lcd.println(sensor);
  M5.Lcd.println(latitude, 6);
  M5.Lcd.println(longitude, 6);
  M5.Lcd.println("TESTTTTTT");
  
  listDir(SD,"/",0);*/
  //delay(10000);
  
  printScan();
        M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0,0);
}

void loop() {
  // update button state
  //M5.update();
  // Look for new cards, and select one if present
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }
  
  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  
  // Dump UID
  Serial.print(F("Card UID:"));
  M5.Lcd.println(" ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.println();
  /*if (M5.BtnA.wasPressed() && state != search)
  {
    state = search;
    printSearch();
  } else if (M5.BtnB.wasPressed() && state != scan)
  {
    if(i_init == 0){
      ble BLE;
    
      M5.Lcd.clearDisplay();
      M5.Lcd.setCursor(110, 50);
      M5.Lcd.println("SEARCH");
      M5.Lcd.setCursor(20, 200);
      M5.Lcd.println("Bitte warten...");
      M5.Lcd.setCursor(70,100);
      M5.Lcd.fillRect(100,100,100,10,0);
      check(BLE);
      i_init = BLE.init;
    
    }
    
  } else if (M5.BtnC.wasPressed() && state == search)
  {
    state = scan;
    printScan();
  } else if (M5.BtnB.wasPressed() && state == scan)
  {
    //TODO rfid scan funktion
    M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0,0);
    if (SD.exists(tokensFile) == true)
    {
      M5.Lcd.print("TEEST3");
      if ( ! rfid.PICC_IsNewCardPresent() || ! rfid.PICC_ReadCardSerial())
      {
        M5.Lcd.print("TEEST4");
        return;
      }
      M5.Lcd.print("TEEST2");

      Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // Check is the PICC of Classic MIFARE type
      if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
          piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
          piccType != MFRC522::PICC_TYPE_MIFARE_4K) 
      {
            Serial.println(F("Your tag is not of type MIFARE Classic."));
            return;
      }

      if (rfid.uid.uidByte[0] != nuidPICC[0] || 
          rfid.uid.uidByte[1] != nuidPICC[1] || 
          rfid.uid.uidByte[2] != nuidPICC[2] || 
          rfid.uid.uidByte[3] != nuidPICC[3] )
      {
          Serial.println(F("A new card has been detected."));
          
          // Store NUID into nuidPICC array
          for (byte i = 0; i < 4; i++) {
          nuidPICC[i] = rfid.uid.uidByte[i];
          }
   
          M5.Lcd.println(F("The NUID tag is:"));
          M5.Lcd.print(F("In hex: "));
          printHex(rfid.uid.uidByte, rfid.uid.size);
          M5.Lcd.println();
          M5.Lcd.print(F("In dec: "));
          printDec(rfid.uid.uidByte, rfid.uid.size);
          M5.Lcd.println();
      } else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
    M5.Lcd.print("TEEST");
    delay(1000);
    printScan();
  } else 
    {
     Serial.println("Datei nicht gefunden.");
    }

  }*/
}



/*
      if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
        //delay(50);
        return;
      }

        M5.Lcd.fillRect(0 ,140, 320, 50 ,0);
        M5.Lcd.setCursor(70,150);
        String test;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        //M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //test += (mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        //M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
        //test += mfrc522.uid.uidByte[i];
        M5.Lcd.print(mfrc522.uid.uidByte[i]);
      }
      //M5.Lcd.println(test);
      //test.clear();
      */


