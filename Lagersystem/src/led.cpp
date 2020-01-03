#include <Arduino.h>
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