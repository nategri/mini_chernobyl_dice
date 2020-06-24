#ifndef SETTINGS_DIAL_H
#define SETTINGS_DIAL_H

#include "display.h"
#include "RotaryEncoder.h"

#define SETTINGS_BUTTON 2
#define SETTINGS_RT1 10
#define SETTINGS_RT2 9

class SettingsDial {
  private:
    LedScreen* ledScreen;
    RotaryEncoder* rotaryEncoder;
    uint8_t diceNum;
    uint8_t diceSize;
  public:
    SettingsDial(LedScreen*);
    uint8_t buttonPushed();
    void handleInput();
    uint8_t getDiceNum();
    uint8_t getDiceSize();
};

#endif
