#include "display.h"
#include "speaker.h"

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define STATUS_GREEN 5
#define STATUS_RED A5

#define SPEAKER_TOGGLE 0

volatile uint8_t didTrigger;
volatile unsigned long prevTrigTime;
volatile unsigned long trigCount;

// Ring buffer for Von Neumann algorithm based algorithm 
volatile uint8_t vnRingBuff[256];
volatile uint8_t vnRingBuffWriteHead;

// Global variables for state machine
//uint8_t turboMode;
uint8_t speakerOn;

LedScreen* ledScreen;
Speaker* speaker;


void geigerEvent() {
  digitalWrite(UV_LED_PIN, HIGH);

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
