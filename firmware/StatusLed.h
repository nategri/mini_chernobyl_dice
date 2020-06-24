#ifndef STATUS_LED_H

#include <Arduino.h>

#define STATUS_GREEN 5
#define STATUS_RED A5

class StatusLed {
  private:
    long unsigned int msTime;
    uint8_t redState;
  public:
    StatusLed();
    void green();
    void red();
    void blinkRed();
};

#endif
