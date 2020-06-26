#include "QuantumRNG.h"

QuantumRNG::QuantumRNG(LedScreen* ledScreen, volatile uint8_t* ringBuff, volatile uint8_t* ringBuffWriteHead) {
  this->ledScreen = ledScreen;
  this->ringBuff = ringBuff;
  this->ringBuffWriteHead = ringBuffWriteHead;
}

void QuantumRNG::burnIn(volatile unsigned long* trigCount) {
  unsigned long startingTrigCount = *trigCount;
  while((*trigCount - startingTrigCount) < 20) {
    int count = *trigCount - startingTrigCount;
    delay(10);
    //char* displayDigits = LedScreen::number_to_digits(count, 0);
    this->ledScreen->animation(count);
  }
  this->ledScreen->clear();
}


uint8_t QuantumRNG::getByte() {
  
  uint8_t randByte = 0;

  uint8_t vnRingBuffReadHead = *(this->ringBuffWriteHead) - 2;

  uint8_t currBit = 0;

  char digits[] = {-1, -1, -1, -1, -1, -1, -1, -1};
  
  this->ledScreen->display(digits);

  while(currBit < 8) {

    uint8_t bitContribution = 0b00000001 << currBit;
    
    while(1) {
      uint8_t diff = *(this->ringBuffWriteHead) - vnRingBuffReadHead;
      //Serial.println(currBit);
      
      if(diff >= 2) {
        // Generate a bit
        uint8_t idx = vnRingBuffReadHead;
        vnRingBuffReadHead += 2;

        if(this->ringBuff[idx] == this->ringBuff[idx+1]) {
          continue;
        }
        else if((this->ringBuff[idx] == 1) && (this->ringBuff[idx+1] == 0)) {
          randByte += bitContribution;
          uint8_t parity = (micros() / 4) % 2;
          randByte = randByte ^ (parity << currBit);
          currBit++;
          break;
        }
        else if((this->ringBuff[idx] == 0) && (this->ringBuff[idx+1] == 1)) {
          uint8_t parity = (micros() / 4) % 2;
          randByte = randByte ^ (parity << currBit);
          currBit++;
          break;
        }
      }    
    }
    
    uint8_t newBit = (randByte & bitContribution) >> (currBit-1);

    digits[currBit-1] = newBit;

    (*this->ledScreen).display(digits);
  }

  return randByte;
}

uint8_t QuantumRNG::getDice(uint8_t diceSize) {
  uint8_t randByte;
  while(1) {
    randByte = this->getByte();
    if(randByte < (256 - (256 % diceSize))) {
        break;
    }
  }
  return (randByte % diceSize);
}
