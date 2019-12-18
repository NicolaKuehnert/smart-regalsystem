#include <Arduino.h>
#include "led.h"

led::led(int lpin){
    pin = lpin;
}

int led::init(){
    pinMode (pin, OUTPUT);
}

int led::on(){
    digitalWrite (pin, HIGH);
}

int led::off(){
    digitalWrite (pin, LOW);
}