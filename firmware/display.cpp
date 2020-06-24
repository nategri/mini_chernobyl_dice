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
  this->ledControl->setIntensity(0,2);
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


void LedScreen::displayVolts(char* digit) {
  LedScreen:display(digit);
  this->ledControl->setDigit(0, 3, digit[3], true);
}

void LedScreen::clear() {
  for(char i=0; i<8; i++) {
    this->ledControl->setChar(0, i, ' ', false);
  }
}

void LedScreen::displaySettings(uint8_t diceNum, uint8_t diceSize) {
  this->ledControl->clearDisplay(0);
  this->ledControl->setDigit(0, 0, diceSize - (diceSize/10), false);
  this->ledControl->setDigit(0, 1, (diceSize/10), false);
  this->ledControl->setChar(0, 2, 'd', false);
  this->ledControl->setDigit(0, 3, diceNum, false);
}

void LedScreen::displayRolls(uint8_t* rolls) {
  uint8_t digitIdx = 0;
  for(uint8_t i = 0; i < 4; i++) {
    if(rolls[i] >= 0) {
      uint8_t tensDigit = (rolls[i]/10);
      uint8_t onesDigit = rolls[i] - tensDigit;
      this->ledControl->setDigit(0, digitIdx, onesDigit, false);
      if(onesDigit > 0) {
        this->ledControl->setDigit(0, digitIdx+1, tensDigit, true);
      }
      else {
        this->ledControl->setChar(0, digitIdx+1, ' ', true);
      }
    }
    digitIdx += 2;
  }
}

void LedScreen::sleep() {
  this->ledControl->shutdown(0, true);
}

void LedScreen::wake() {
  this->ledControl->shutdown(0, false);
  this->ledControl->setIntensity(0, 2);
  this->ledControl->clearDisplay(0);
}
