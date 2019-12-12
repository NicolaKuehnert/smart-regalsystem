#include <Arduino.h>
#include <M5Stack.h>
#include "printDisplay.h"
#include "search.h"
#include "ble.h"

State state = scan;
uint32_t targetTime = 0;
int i_init = 0;


#define sleeptime 1

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

void setup() {
  M5.begin(true, false, true);
 
  M5.Power.begin();
  
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
    state = scan;
    printScan();
  }
}



