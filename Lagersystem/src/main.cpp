#include <Arduino.h>
#include <M5Stack.h>
#include "printDisplay.h"
#include "search.h"

State state = scan;

/*
*Printet das Menüfeld der Searchfunktion aus.
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
     M5.Lcd.setCursor(110, 100);
    M5.Lcd.println("verbunden");
  }
  M5.Lcd.setCursor(5, 200);
  M5.Lcd.println("Search");
  M5.Lcd.setCursor(230, 200);
  M5.Lcd.println("Scan");
  
}
/*
*Printet das Menüfeld der Scanfunktion aus.
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
    
  } else if (M5.BtnC.wasPressed() && state != scan)
  {
    state = scan;
    printScan();
  }
}

