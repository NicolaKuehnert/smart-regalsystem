class led {
    public:
    led(int lpin);
    int init();
    int on();
    int off();
    private:
    int pin;
}