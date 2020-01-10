#ifndef LED_H
#define LED_H

#include <Arduino.h>
#include <M5Stack.h>

#define sleeptime 1
class led {
    public:
    led(int lpin);
    int init();
    int on();
    int off();
    private:
    int pin;
};
void redLed();
void yellowLed();
#endif