#include <Arduino.h>
#include <M5Stack.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <ArduinoJson.h>

#include "printDisplay.h"
#include "search.h"
#include "ble.h"
#include "MFRC522_I2C.h"
#include "led.h"




State state = scan;
uint32_t targetTime = 0;
int i_init = 0;
// 0x28 is i2c address on SDA. Check your address with i2cscanner if not match.
MFRC522 mfrc522(0x28);   // Create MFRC522 instance.
#define sleeptime 1
const char *filename = "/datenbank.txt";
String scannedTag= "";
String rowLast = "";
String row1 = " 04 6d 95 a2 ec 5a 81";
String row2 = " 04 67 95 a2 ec 5a 81";
bool rowScanned = false;

ble BLE;
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

/*******************************************
 *      FUNKTIONEN FÜR BLUETOOTH
 ******************************************/
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
bool saveTag(const char *filename, String &row, String book) {
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

  file.close();

  for (int i = 0; i < doc[row1].size(); i++)
  {
    //M5.Lcd.println(doc[row].getMember(i));
    if (doc[row1].getElement(i) == book)
    {
      M5.Lcd.println("gibes schon");
      return false;
    }
  }

  for (int i = 0; i < doc[row2].size(); i++)
  {
   
    //M5.Lcd.println(doc[row].getMember(i));
    if (doc[row2].getElement(i) == book)
    {
      M5.Lcd.println("gibes schon");
      return false;
    }
  }
  file = SD.open(filename, FILE_WRITE);
  doc[row].add(book);
  if (serializeJson(doc, file) == 0) {
    M5.Lcd.println(F("Failed to write to file"));
  }
  file.close();
  return true;
}

void searchTag(const char *filename, String &book)
{

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

  M5.Lcd.setCursor(0, 150);
  file.close();
  if (doc[row1].getMember(book) != NULL)
  {
    M5.Lcd.println("reihe 1");
    //redLed();
  }
  else if (doc[row2].getMember(book) != NULL)
  {
    //yellowLed();
    M5.Lcd.println("reihe 2");
  }
  else
  {
    M5.Lcd.println("gibt es nicht");
  }
  
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    M5.Lcd.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    M5.Lcd.print((char)file.read());
  }
  M5.Lcd.println();

  // Close the file
  file.close();
}

/**************************************
 *      FUNKTIONEN FÜR DIE LED
 *************************************/
void redLed()
{
led red(5);
  led yellow(2);

  try{
    int status = red.init();
    if(status == 1){
      throw 101;
    }
    status = yellow.init();
    if(status == 1){
      throw 102;
    }

    red.on();
    sleep(sleeptime);
    red.off();
    yellow.on();
    sleep(sleeptime);
    yellow.off();

  } catch(int i) {

    switch(i){
      case 101:
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(0,0);
        M5.Lcd.println("Error 101: Red LED not initialised");
        break;
      case 102:
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(0,0);
        M5.Lcd.println("Error 102: Yellow LED not initialised");
        break;
    }

  }
}

void yellowLed()
{
led red(5);

  try{
    int status = red.init();
    if(status == 1){
      throw 101;
    }
    red.on();
    sleep(sleeptime);
    red.off();

  } catch(int i) {

    switch(i){
      case 101:
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(0,0);
        M5.Lcd.println("Error 101: Red LED not initialised");
        break;
    }

  }
}
void ledtest(){
  
  led yellow(2);

  try{
    int status = yellow.init();
    if(status == 1){
      throw 102;
    }

    yellow.on();
    sleep(sleeptime);
    yellow.off();

  } catch(int i) {

    switch(i){
      case 102:
        M5.Lcd.clearDisplay();
        M5.Lcd.setCursor(0,0);
        M5.Lcd.println("Error 102: Yellow LED not initialised");
        break;
    }

  }
}

void scanTag()
{
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  M5.Lcd.println(" ");
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    scannedTag += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    scannedTag += String(mfrc522.uid.uidByte[i], HEX);
  }

  if ( rowScanned == false)
  {
    M5.Lcd.setCursor(0, 150);
    M5.Lcd.setTextSize(2);
    M5.Lcd.print("erster Scan: ");
    M5.Lcd.setCursor(145, 150);
    if (scannedTag == row1)
    {
      M5.Lcd.println("Reihe 1");
      rowScanned = true;
      rowLast = scannedTag;
    } else if (scannedTag == row2)
    {
      M5.Lcd.println("Reihe 2");
      rowScanned = true;
      rowLast = scannedTag;
    } else
    {
      M5.Lcd.print("Reihe nicht gefunden");
    }
  } else if (rowScanned == true)
  {
    M5.Lcd.setCursor(0, 170);
    M5.Lcd.print("zweiter Scan: ");
    if (scannedTag != row1 && scannedTag != row2) 
    {
      if(saveTag(filename, rowLast, scannedTag) == true)
      {
        M5.Lcd.setCursor(150, 170);
        M5.Lcd.print(scannedTag);
          //printFile(filename);
        rowScanned = false;
      }
    } else 
    {
      M5.Lcd.setCursor(150, 170);
      M5.Lcd.print("Kein Ordner");
    }
   }
    scannedTag.clear();
    delay(1000);
}
/**************************************
 *      MAIN PROGRAMM
 *************************************/
void setup() {
  M5.begin(true, true, true);
  M5.Power.begin();
  Wire.begin();
  mfrc522.PCD_Init();
  
  // Initialize SD library
  while (!SD.begin()) {
    Serial.println(F("Failed to initialize SD library"));
    delay(1000);
  }
  SD.remove(filename);
  if (! SD.exists(filename))
  {
    StaticJsonDocument<200> doc;
    char json[] =
      "{\" 04 6d 95 a2 ec 5a 81\":[],\" 04 67 95 a2 ec 5a 81\":[]}";

    // Deserialize the JSON document
    deserializeJson(doc, json);
    // Open file for writing
    File file = SD.open(filename, FILE_WRITE);
    if (serializeJson(doc, file) == 0) {
      M5.Lcd.println(F("Failed to write to file"));
    }
    file.close();
  }
 
  

 

  
  printScan();

}

void loop() {
  // update button state
  M5.update();

  if (M5.BtnA.wasPressed() && state != search)
  {
    state = search;
    printSearch();
    String test =  " 04 4f 95 a2 ec 5a 81";
    searchTag(filename, test);
  } else if (M5.BtnB.wasPressed() && state != scan)
  {
    if(i_init == 0){
      BLE.init();
      i_init = 1;
    }
    M5.Lcd.clearDisplay();
    M5.Lcd.setCursor(110, 50);
    M5.Lcd.println("SEARCH");
    M5.Lcd.setCursor(20, 200);
    M5.Lcd.println("Bitte warten...");
    M5.Lcd.setCursor(70,100);
    M5.Lcd.fillRect(100,100,100,10,0);
    check(BLE);
    
    
    
  } else if (M5.BtnC.wasPressed() && state == search)
  {
    state = scan;
    printScan();
  } else if (M5.BtnB.wasPressed() && state == scan)
  {
    scanTag();
  }
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


