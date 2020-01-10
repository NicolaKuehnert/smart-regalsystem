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
const char *filename = "/datenbank.txt";
String scannedTag= "";

ble BLE;

/*
*Gibt das Menüfeld der Searchfunktion aus.
*Diese Methode wird ausgeführt wen der "A-Knopf" gedrückt wird und der Zustand >nicht< auf "search" ist.
*/
void printSearch(){

  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.println("Scan");
  M5.Lcd.setTextColor(PINK);
  M5.Lcd.setCursor(110, 50);
  M5.Lcd.println("SEARCH");
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("Search");

  
}
/*
*Gibt das Menüfeld der Scanfunktion aus.
*Diese Methode wird ausgeführt wen der "B-Knopf" gedrückt wird und der Zustand >nicht< auf "scan" ist.
*/
void printScan(){
  
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(PINK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("Search");
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setCursor(110, 50);
  M5.Lcd.println("SCAN");
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.println("Del");
  M5.Lcd.setCursor(150, 200);
  M5.Lcd.println("Add");

  
}

void printDelete()
{
  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextColor(YELLOW);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(110, 50);
  M5.Lcd.println("SCAN");
  M5.Lcd.setCursor(0, 100);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.println("NEIN");
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("JA");
  M5.Lcd.setTextColor(YELLOW);
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

/**************************************
 *      MAIN PROGRAMM
 *************************************/
void setup() {
  M5.begin(true, true, true);
  M5.Power.begin();
  Wire.begin();
  rfidInit();
  
  // Initialize SD library
  while (!SD.begin()) {
    Serial.println(F("Failed to initialize SD library"));
    delay(1000);
  }
  if (!SD.exists(filename))
  {
    StaticJsonDocument<200> doc;
    char json[] =
      "{\"Reihe1\":[],\"Reihe2\":[]}";

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


