#include "SettingsDial.h"

SettingsDial::SettingsDial(LedScreen* ledScreen) {
  this->ledScreen = ledScreen;
  this->rotaryEncoder = new RotaryEncoder(SETTINGS_RT1, SETTINGS_RT2);
  pinMode(SETTINGS_BUTTON, INPUT_PULLUP);
  this->ms = 0;
  this->state = 0;
  this->lastState = 0;

  // Default settings
  this->diceNum = 1;
  this->diceSize = 6;
}

uint8_t SettingsDial::buttonPressed(uint16_t pressInterval) {
  
  uint8_t returnState = 0;
  
  uint8_t reading = !digitalRead(SETTINGS_BUTTON);

  if(reading != this->lastState) {
    this->ms = millis();
  }

  if((millis() - this->ms) > pressInterval) {
    if(reading != this->state) {
      this->state = reading;
    }

    if(this->state == 1) {
      returnState = 1;
    }
  }

  this->lastState = reading;

  return returnState;
}

uint8_t SettingsDial::buttonShortPressed() {
  return this->buttonPressed(30);
}

uint8_t SettingsDial::buttonLongPressed() {
  return this->buttonPressed(3000);
}

void SettingsDial::handleInput() {
  delay(200);

  // Set number of dice
  while(1) {
    if(this->buttonShortPressed()) {
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

    this->ledScreen->displaySettings(this->diceNum, this->diceSize, 1, 0);
  }
  delay(200);

  // Set size of dice
  while(1) {
    if(this->buttonShortPressed()) {
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

    this->ledScreen->displaySettings(this->diceNum, this->diceSize, 0, 1);
  }
  this->ledScreen->clear();
  delay(200);
}

uint8_t SettingsDial::getDiceNum() {
  return this->diceNum;
}

uint8_t SettingsDial::getDiceSize() {
  return this->diceSize;
}
