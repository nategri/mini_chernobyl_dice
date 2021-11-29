#ifndef SETTINGS_DIAL_H
#define SETTINGS_DIAL_H

#include "display.h"
#include "RotaryEncoder.h"

class SettingsDial {
  private:
    LedScreen* ledScreen;
    RotaryEncoder* rotaryEncoder;
    uint8_t diceNum;
    uint8_t diceSize;
    unsigned long int ms;
    uint8_t lastState;
    uint8_t state;
    uint8_t buttonPressed(uint16_t);
  public:
    SettingsDial(LedScreen*);
    uint8_t buttonLongPressed();
    uint8_t buttonShortPressed();
    void handleInput();
    uint8_t getDiceNum();
    uint8_t getDiceSize();
};

#endif
