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

  this->blinkMsTime = millis();
  this->blinkState = 0;
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

void LedScreen::displaySettings(uint8_t diceNum, uint8_t diceSize, uint8_t flashNum, uint8_t flashSize) {
  unsigned long int currBlinkMsTime = millis();
  
  uint8_t diceSizeTensDigit = (diceSize/10);
  uint8_t diceSizeOnesDigit = diceSize - 10*diceSizeTensDigit;

  if((currBlinkMsTime - this->blinkMsTime) > 25) {
    this->blinkState = !this->blinkState;
    this->blinkMsTime = currBlinkMsTime;
  }

  if(!this->blinkState && flashSize) {
    this->ledControl->setChar(0, 0, ' ', false);
    this->ledControl->setChar(0, 1, ' ', false);
  }
  else {
    this->ledControl->setDigit(0, 0, diceSizeOnesDigit, false);
    this->ledControl->setDigit(0, 1, diceSizeTensDigit, false);    
  }

  if(!this->blinkState && flashNum) {
    this->ledControl->setChar(0, 3, ' ', false);
  }
  else {
    this->ledControl->setChar(0, 2, 'd', false);
    this->ledControl->setDigit(0, 3, diceNum, false);
  }
}

void LedScreen::displayRolls(char* rolls) {
  this->clear();
  uint8_t digitIdx = 0;
  for(uint8_t i = 0; i < 4; i++) {
    if(rolls[i] >= 0) {
      
      uint8_t tensDigit = (rolls[i]/10);
      uint8_t onesDigit = rolls[i] - 10*tensDigit;
      
      this->ledControl->setDigit(0, digitIdx, onesDigit, true);
      if(tensDigit > 0) {
        this->ledControl->setDigit(0, digitIdx+1, tensDigit, false);
      }
      else {
        this->ledControl->setChar(0, digitIdx+1, ' ', false);
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
