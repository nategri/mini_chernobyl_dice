#ifndef DISPLAY_H
#define DISPLAY_H

#include "LedControl.h"

class LedScreen {
  private:
    LedControl* ledControl;
    unsigned long int blinkMsTime;
    uint8_t blinkState;
  public:
    static char* number_to_digits(unsigned long num, const unsigned char zero_pad);
    LedScreen();
    void display(char*);
    void animation(uint8_t);
    void displayVolts(char*);
    void displaySettings(uint8_t, uint8_t, uint8_t, uint8_t);
    void displayRolls(char*);
    void clear();
    void sleep();
    void wake();
};

#endif
