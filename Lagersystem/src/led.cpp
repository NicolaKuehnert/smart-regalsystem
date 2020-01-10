#include "led.h"

led::led(int lpin){
    pin = lpin;
}

int led::init(){
    pinMode(pin, OUTPUT);
    if(GPIO_IS_VALID_OUTPUT_GPIO(pin)){
        return 0;
    } else {
        return 1;
    }
}

int led::on(){
    digitalWrite (pin, HIGH);
}

int led::off(){
    digitalWrite (pin, LOW);
}

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