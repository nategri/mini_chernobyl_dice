#ifndef DISPLAY_H
#define DISPLAY_H

#include "LedControl.h"

#define LED_DISP_DATAIN 11
#define LED_DISP_CLK 13
#define LED_DISP_LOAD 7

class LedScreen {
  private:
    LedControl* ledControl;
  public:
    static char* number_to_digits(unsigned long num, const unsigned char zero_pad);
    LedScreen();
    void display(char*);
    void clear();
};

#endif
