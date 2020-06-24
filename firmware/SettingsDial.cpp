#include "SettingsDial.h"

SettingsDial::SettingsDial(LedScreen* ledScreen) {
  this->ledScreen = ledScreen;
  this->rotaryEncoder = new RotaryEncoder(SETTINGS_RT1, SETTINGS_RT2);
  pinMode(SETTINGS_BUTTON, INPUT_PULLUP);

  // Default settings
  this->diceNum = 1;
  this->diceSize = 6;
}

uint8_t SettingsDial::buttonPushed() {
  if(digitalRead(SETTINGS_BUTTON) == LOW) {
    return 1;
  }
  else {
    return 0;
  }
}

void SettingsDial::handleInput() {

  // Set number of dice
  while(1) {
    if(this->buttonPushed()) {
      delay(100);
      break;
    }

    this->rotaryEncoder->tick();
    RotaryEncoder::Direction dir = this->rotaryEncoder->getDirection();

    if(dir == RotaryEncoder::Direction::CLOCKWISE) {
      if(this->diceNum < 4) {
        this->diceNum++;
      }
    }
    else if(dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
      if(this->diceNum > 1) {
        this->diceNum--;
      }
    }

    this->ledScreen->displaySettings(this->diceNum, this->diceSize);
  }

  // Set size of dice
  while(1) {
    if(this->buttonPushed()) {
      delay(100);
      break;
    }

    this->rotaryEncoder->tick();
    RotaryEncoder::Direction dir = this->rotaryEncoder->getDirection();

    if(dir == RotaryEncoder::Direction::CLOCKWISE) {
      if(this->diceSize < 20) {
        this->diceSize++;
      }
    }
    else if(dir == RotaryEncoder::Direction::COUNTERCLOCKWISE) {
      if(this->diceSize > 1) {
        this->diceSize--;
      }
    }

    this->ledScreen->displaySettings(this->diceNum, this->diceSize);
  }
}

uint8_t SettingsDial::getDiceNum() {
  return this->diceNum;
}

uint8_t SettingsDial::getDiceSize() {
  return this->diceSize;
}
