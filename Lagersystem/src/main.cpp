#include <Arduino.h>
#include <M5Stack.h>
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <ArduinoJson.h>

#include "scan.h"
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
String string1scan = "";
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

void printDelete()
{
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
  M5.Lcd.println("NEIN");
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("JA");
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

void searchTag(const char *filename, std::string &book)
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
  if (doc[row1].getMember(book.c_str()) != NULL)
  {
    M5.Lcd.println("reihe 1");
    redLed();
  }
  else if (doc[row2].getMember(book.c_str()) != NULL)
  {
    yellowLed();
    M5.Lcd.println("reihe 2");
  }
  else
  {
    M5.Lcd.println("gibt es nicht");
  }
  
}

bool deleteTag(const char *filename, String &book)
{
  File file = SD.open(filename);
  StaticJsonDocument<520> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  M5.Lcd.setCursor(0, 150);
  file.close();
  if (doc[row1].getMember(book) != NULL)
  {
    doc[row1].remove(book);
  }
  else if (doc[row2].getMember(book) != NULL)
  {
    doc[row2].remove(book);
  }
  else
  {
    M5.Lcd.println("gibt es nicht");
    return false;
  }
  file = SD.open(filename, FILE_WRITE);
  if (serializeJson(doc, file) == 0) {
    M5.Lcd.println(F("Failed to write to file"));
  }
  file.close();
  return true;
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

void scanTag(int option)
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
  if (option == 0)
  {
    if (rowScanned == false)
    {
      printScan();
      M5.Lcd.setCursor(0, 150);
      M5.Lcd.setTextSize(2);
      M5.Lcd.print("erster Scan: ");
      M5.Lcd.setCursor(145, 150);
      if (scannedTag == row1)
      {
        string1scan = "Reihe 1";
        rowScanned = true;
        rowLast = scannedTag;
      } else if (scannedTag == row2)
      {
        string1scan = "Reihe 2";
        rowScanned = true;
        rowLast = scannedTag;
      } else
      {
        string1scan = "Reihe nicht gefunden";
      }
      M5.Lcd.print(string1scan);
    } else if (rowScanned == true)
    {
      printScan();
      M5.Lcd.setCursor(0, 150);
      M5.Lcd.setTextSize(2);
      M5.Lcd.print("erster Scan: ");
      M5.Lcd.setCursor(145, 150);
      M5.Lcd.print(string1scan);
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
  } else if(option == 1)
  {
    printDelete();
    M5.Lcd.setCursor(0, 150);
    M5.Lcd.setTextSize(2);
    if(scannedTag != row1 && scannedTag != row2)
    {
      M5.Lcd.println(scannedTag);
      M5.Lcd.println("wirklich löschen?");
      if(M5.BtnA.wasPressed())
      {
        deleteTag(filename, scannedTag);
      } else if (M5.BtnC.wasPressed())
      {
        printScan();
      }
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
    scanTag(0);
  } else if (M5.BtnC.wasPressed() && state == scan)
  {
    scanTag(1);
  }
}


