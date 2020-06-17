#include "display.h"

static char* LedScreen::number_to_digits(unsigned long num, const unsigned char zero_pad) {
  static char digits[8];
  for(int i=0; i<8; i++) {
    unsigned long pow10 = pow(10, i) + 0.5;
    digits[i] = (num / pow10) % 10;
  }

  if(!zero_pad) {
    // Clear leading zeros
    for(int i=7; i>=1; i--) {
      if(digits[i] != 0) {
        break;
      }
      else {
        digits[i] = -1;
      }
    }
  }

  return digits;
}

LedScreen::LedScreen() {
  this->ledControl = new LedControl(LED_DISP_DATAIN, LED_DISP_CLK, LED_DISP_LOAD, 1);
  this->ledControl->shutdown(0, false);
  this->ledControl->setIntensity(0,1);
  this->ledControl->clearDisplay(0);
}

void LedScreen::display(char* digit) {
  for(char i=0; i<8; i++) {
    if(digit[i] < 0) {
      this->ledControl->setChar(0, i, ' ', false);
    }
    else {
      this->ledControl->setDigit(0, i, digit[i], false);
    }
  }
}

void LedScreen::clear() {
  for(char i=0; i<8; i++) {
    this->ledControl->setChar(0, i, ' ', false);
  }
}
