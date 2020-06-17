#ifndef SPEAKER_H
#define SPEAKER_H

#include <Arduino.h>

#define SPEAKER_PIN_A A1
#define SPEAKER_PIN_B A0

class Speaker {
  public:
    Speaker();
    void clickBegin();
    void clickEnd();
};

#endif
