#include <Arduino.h>
#include "Keyswitch.h"
#include "pins.h"

Keyswitch::Keyswitch() {
  pinMode(KEYSWITCH_PIN, INPUT_PULLUP);
  this->ms = 0;
  this->state = 0;
  this->lastState = 0;
}

uint8_t Keyswitch::pressed() {
  
  uint8_t returnState = 0;
  
  uint8_t reading = !digitalRead(KEYSWITCH_PIN);

  if(reading != this->lastState) {
    this->ms = millis();
  }

  if((millis() - this->ms) > 30) {
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
