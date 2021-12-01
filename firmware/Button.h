#ifndef BUTTON_H
#define BUTTON_H

class Button {
  private:
    unsigned long int ms;
    uint8_t lastState;
    uint8_t state;
    uint8_t pressed(uint16_t);
    uint8_t pin;
  public:
    Button(uint8_t);
    uint8_t shortPressed();
    uint8_t longPressed();
};

#endif
