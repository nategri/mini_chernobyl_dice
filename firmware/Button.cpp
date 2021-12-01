#include <Arduino.h>
#include "Button.h"
#include "pins.h"

Button::Button(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
  this->ms = 0;
  this->state = 0;
  this->lastState = 0;
  this->pin = pin;
}

uint8_t Button::pressed(uint16_t pressInterval) {
  
  uint8_t returnState = 0;
  
  uint8_t reading = !digitalRead(this->pin);

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

uint8_t Button::shortPressed() {
  return this->pressed(30);
}

uint8_t Button::longPressed() {
  return this->pressed(3000);
}
