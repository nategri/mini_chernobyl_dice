#include "StatusLed.h"

StatusLed::StatusLed() {
  pinMode(STATUS_GREEN, OUTPUT);
  pinMode(STATUS_RED, OUTPUT);
  this->msTime = millis();
  this->redState = HIGH;
}

void StatusLed::red() {
  digitalWrite(STATUS_GREEN, LOW);
  digitalWrite(STATUS_RED, HIGH);
}

void StatusLed::green() {
  digitalWrite(STATUS_RED, LOW);
  digitalWrite(STATUS_GREEN, HIGH);
}

void StatusLed::blinkRed() {
  digitalWrite(STATUS_GREEN, LOW);
  long unsigned int currMsTime = millis();
  if((currMsTime - this->msTime) > 1000) {
    this->redState = !this->redState;
    digitalWrite(STATUS_RED, this->redState);
    this->msTime = currMsTime;
  }
}

void StatusLed::off() {
  digitalWrite(STATUS_GREEN, LOW);
  digitalWrite(STATUS_RED, LOW);
}
