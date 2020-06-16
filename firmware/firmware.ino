#include "LedControl.h"

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define STATUS_GREEN 5
#define STATUS_RED A5

#define LED_DISP_DATAIN 11
#define LED_DISP_CLK 13
#define LED_DISP_LOAD 7

#define SPEAKER_PIN_A A1
#define SPEAKER_PIN_B A0

#define SPEAKER_TOGGLE 1

volatile uint8_t didTrigger;
volatile unsigned long prevTrigTime;
volatile unsigned long trigCount;

// Ring buffer for Von Neumann algorithm based algorithm 
volatile uint8_t vnRingBuff[256];
volatile uint8_t vnRingBuffWriteHead;

// Global variables for state machine
//uint8_t turboMode;
uint8_t speakerOn;
uint8_t trigLedsOn;

//
// SPEAKER CLASS
//

class Speaker {
  public:
    Speaker();
    void clickBegin();
    void clickEnd();
};

Speaker::Speaker() {
  pinMode(SPEAKER_PIN_A, OUTPUT);
  pinMode(SPEAKER_PIN_B, OUTPUT);

  digitalWrite(SPEAKER_PIN_A, LOW);
  digitalWrite(SPEAKER_PIN_B, LOW);
}

void Speaker::clickBegin() {
  digitalWrite(SPEAKER_PIN_A, HIGH);
  digitalWrite(SPEAKER_PIN_B, LOW);
}

void Speaker::clickEnd() {
  digitalWrite(SPEAKER_PIN_A, LOW);
  digitalWrite(SPEAKER_PIN_B, LOW);
}

Speaker* speaker;


void geigerEvent() {
  if(trigLedsOn) {
    digitalWrite(UV_LED_PIN, HIGH);
  }

  if(speakerOn) {
    speaker->clickBegin();
  }

  didTrigger = 1;
  prevTrigTime = millis();
  trigCount++;
}

ISR(TIMER0_COMPA_vect) {
  // Turn off UV LEDs if it's been more than 50 ms since last trigger
  // Also a finish a "click" on the piezo speaker if it's been 1 ms or more
  unsigned long now = millis();

  unsigned long dt = now - prevTrigTime;

  if(dt > 50) {
    digitalWrite(UV_LED_PIN, LOW);
  }

  if(dt >= 1) {
    if(speakerOn) {
      speaker->clickEnd();
    }
  }

  // Handle this 1 ms window
  if(didTrigger) {
    vnRingBuff[vnRingBuffWriteHead] = 1;
    didTrigger = 0;
  }
  else {
    vnRingBuff[vnRingBuffWriteHead] = 0;
  }

  vnRingBuffWriteHead++;
}

//
// DISPLAY CLASS
//

class LedScreen {
  private:
    LedControl* ledControl;
  public:
    static char* number_to_digits(unsigned long num, const unsigned char zero_pad);
    LedScreen();
    void display(char*);
    void clear();
};

static char* LedScreen::number_to_digits(unsigned long num, const unsigned char zero_pad) {
  static char digits[8];
  for(int i=0; i<8; i++) {
    unsigned long pow10 = pow(10, i) + 0.5;
    digits[i] = (num / pow10) % 10;
  }

  if(!zero_pad) {
    // Clear leading zeros
    for(int i=7; i>=1; i--) {
      if(digits[i] != 0) {
        break;
      }
      else {
        digits[i] = -1;
      }
    }
  }

  return digits;
}

LedScreen::LedScreen() {
  this->ledControl = new LedControl(LED_DISP_DATAIN, LED_DISP_CLK, LED_DISP_LOAD, 1);
  this->ledControl->shutdown(0, false);
  this->ledControl->setIntensity(0,1);
  this->ledControl->clearDisplay(0);
}

void LedScreen::display(char* digit) {
  for(char i=0; i<8; i++) {
    if(digit[i] < 0) {
      this->ledControl->setChar(0, i, ' ', false);
    }
    else {
      this->ledControl->setDigit(0, i, digit[i], false);
    }
  }
}

void LedScreen::clear() {
  for(char i=0; i<8; i++) {
    this->ledControl->setChar(0, i, ' ', false);
  }
}

//
// END DISPLAY CLASS
//

LedScreen* ledScreen;

uint8_t getRandByte() {
  
  uint8_t randByte = 0;

  uint8_t vnRingBuffReadHead = vnRingBuffWriteHead - 2;

  uint8_t currBit = 0;

  char digits[] = {-1, -1, -1, -1, -1, -1, -1, -1};
  ledScreen->display(digits);

  while(currBit < 8) {

    uint8_t bitContribution = 0b00000001 << currBit;
    
    while(1) {

      uint8_t diff = vnRingBuffWriteHead - vnRingBuffReadHead;
      
      if(diff >= 2) {
        // Generate a bit
        uint8_t idx = vnRingBuffReadHead;
        vnRingBuffReadHead += 2;

        if(vnRingBuff[idx] == vnRingBuff[idx+1]) {
          continue;
        }
        else if((vnRingBuff[idx] == 1) && (vnRingBuff[idx+1] == 0)) {
          randByte += bitContribution;
          uint8_t parity = (micros() / 4) % 2;
          randByte = randByte ^ (parity << currBit);
          currBit++;
          break;
        }
        else if((vnRingBuff[idx] == 0) && (vnRingBuff[idx+1] == 1)) {
          uint8_t parity = (micros() / 4) % 2;
          randByte = randByte ^ (parity << currBit);
          currBit++;
          break;
        }
      }
      
    }
    
    uint8_t newBit = (randByte & bitContribution) >> (currBit-1);

    digits[currBit-1] = newBit;

    //if(!turboMode) {
    ledScreen->display(digits);
    //}
    //else {
      //nixies->display_led(digits);
    //}
  }

  return randByte;
}

void setup() {
  // put your setup code here, to run once:

  vnRingBuffWriteHead = 0;
  prevTrigTime = 0;
  trigCount = 0;
  vnRingBuff[vnRingBuffWriteHead] = 0;

  speakerOn = SPEAKER_TOGGLE;

  // Settings for Timer0 interrupt
  OCR0A = 0xAF; // Count at which to insert interrupt
  TIMSK0 |= _BV(OCIE0A);
  
  ledScreen = new LedScreen();
  //char* displayDigits = LedScreen::number_to_digits(31415926, 0);
  //ledScreen.display(displayDigits);

  speaker = new Speaker();

  // Turn on Geiger board
  pinMode(GEIGER_PWR, OUTPUT);
  digitalWrite(GEIGER_PWR, HIGH);

  // Init LED flash pin
  pinMode(UV_LED_PIN, OUTPUT);

  // Attach geiger counter interrupt
  pinMode(GEIGER_TRG, INPUT_PULLUP);
  attachInterrupt(1, geigerEvent, FALLING);

  trigLedsOn = 1;
}

void loop() {
  // put your main code here, to run repeatedly:

  uint8_t randByte = getRandByte();
  char* displayDigits = LedScreen::number_to_digits(randByte, 0);
  delay(2000);
  ledScreen->display(displayDigits);
  delay(2000);
  ledScreen->clear();
}
