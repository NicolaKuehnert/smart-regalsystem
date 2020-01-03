#ifndef LED_H
#define LED_H
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