#ifndef QUANTUM_RNG
#define QUANTUM_RNG

#include "display.h"

class QuantumRNG {
  private:
    LedScreen* ledScreen;
    volatile uint8_t* ringBuff;
    volatile uint8_t* ringBuffWriteHead;
  public:
    QuantumRNG(LedScreen*, volatile uint8_t*, volatile uint8_t*);
    void burnIn(volatile unsigned long*);
    uint8_t getByte();
    uint8_t getDice(uint8_t);
};

#endif
