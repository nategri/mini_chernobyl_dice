#ifndef STATUS_LED_H

#include <Arduino.h>

class StatusLed {
  private:
    long unsigned int msTime;
    uint8_t redState;
  public:
    StatusLed();
    void green();
    void red();
    void blinkRed();
    void off();
};

#endif
