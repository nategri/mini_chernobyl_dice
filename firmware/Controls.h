#ifndef SETTINGS_DIAL_H
#define SETTINGS_DIAL_H

#include "display.h"
#include "RotaryEncoder.h"
#include "Button.h"

class Controls {
  private:
    LedScreen* ledScreen;
    RotaryEncoder* rotaryEncoder;
    uint8_t diceNum;
    uint8_t diceSize;
  public:
    Controls(LedScreen*);
    Button* keyswitchButton;
    Button* dialButton;
    void handleDiceInput();
    uint8_t getDiceNum();
    uint8_t getDiceSize();
};

#endif
