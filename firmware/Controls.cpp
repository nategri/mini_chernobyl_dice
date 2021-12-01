#include "Controls.h"
#include "pins.h"

Controls::Controls(LedScreen* ledScreen) {
  this->ledScreen = ledScreen;
  this->rotaryEncoder = new RotaryEncoder(SETTINGS_RT1, SETTINGS_RT2);

  this->keyswitchButton = new Button(KEYSWITCH_PIN);
  this->dialButton = new Button(SETTINGS_BUTTON);

  // Default settings
  this->diceNum = 1;
  this->diceSize = 6;
}

void Controls::handleDiceInput() {
  delay(200);

  // Set number of dice
  while(1) {
    if(this->dialButton->shortPressed()) {
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
    if(this->dialButton->shortPressed()) {
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

uint8_t Controls::getDiceNum() {
  return this->diceNum;
}

uint8_t Controls::getDiceSize() {
  return this->diceSize;
}
