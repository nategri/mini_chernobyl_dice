#ifndef KEYSWITCH_H
#define KEYSWITCH_H

#define KEYSWITCH_PIN 4

class Keyswitch {
  private:
    unsigned long int ms;
    uint8_t lastState;
    uint8_t state;
  public:
    Keyswitch();
    uint8_t pressed();
};

#endif
