#include <Arduino.h>
#include <M5Stack.h>
#include "printDisplay.h"
#include "search.h"
#include "ble.h"

State state = scan;
uint32_t targetTime = 0;
int i_init = 0;
// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.
#define sleeptime 1
char *tokensFile = "/datenbank.txt";
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

    // Print blank line on screen
    //M5.Lcd.printf(" \n  ");
    //M5.Lcd.printf(" \n  ");
    
    Serial.printf("Listing directory: %s\n", dirname);
    //M5.Lcd.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        //M5.Lcd.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        //M5.Lcd.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            //M5.Lcd.print("  DIR : ");
            Serial.println(file.name());
            //M5.Lcd.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            //M5.Lcd.print("  FILE: ");
            Serial.print(file.name());
            //M5.Lcd.print(file.name());
            Serial.print("  SIZE: ");
            //M5.Lcd.print("  SIZE: ");
            Serial.println(file.size());
            //M5.Lcd.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path) {
    Serial.printf("Reading file: %s\n", path);
    //M5.Lcd.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        //M5.Lcd.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    //M5.Lcd.print("Read from file: ");
    while(file.available()){
        int ch = file.read();
        Serial.write(ch);
        //M5.Lcd.write(ch);
    }
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);
    //M5.Lcd.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        //M5.Lcd.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
        //M5.Lcd.println("File written");
    } else {
        Serial.println("Write failed");
        //M5.Lcd.println("Write failed");
    }
}

void setup() {
  M5.begin(true, true, true);
 
  M5.Power.begin();
  
  Wire.begin();
  writeFile(SD, tokensFile, " ");

  mfrc522.PCD_Init();             // Init MFRC522
  
  printScan();
}

void loop() {
  // update button state
  M5.update();
 
  if (M5.BtnA.wasPressed() && state != search)
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
    
  } else if (M5.BtnC.wasPressed() && state != scan)
  {
    //TODO rfid scan funktion
    if (SD.exists(tokensFile) == true)
    {
      if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
        //delay(50);
        return;
      }

        M5.Lcd.fillRect(0 ,140, 320, 50 ,0);
        M5.Lcd.setCursor(70,150);
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
      }
      
    } else 
    {
     Serial.println("Datei nicht gefunden.");
    }
  }
}



