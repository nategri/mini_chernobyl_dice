// SETTINGS
#define SPEAKER_TOGGLE 0

#include <avr/sleep.h>

#include "display.h"
#include "speaker.h"
#include "QuantumRNG.h"
#include "StatusLed.h"
#include "SettingsDial.h"

//
// Microcontroller pins
//

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define USB_V A3
#define BAT_V A6

//
// Global variables
//

volatile uint8_t didTrigger;
volatile unsigned long prevTrigTime;
volatile unsigned long trigCount;

// Ring buffer for Von Neumann algorithm based algorithm 
volatile uint8_t vnRingBuff[256];
volatile uint8_t vnRingBuffWriteHead;

uint8_t speakerOn;
LedScreen* ledScreen;
Speaker* speaker;
QuantumRNG* quantumRand;
StatusLed* statusLed;
SettingsDial* settingsDial;

//
// Interrupt functions
//

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

void wake_noop() {}

float read_bat_volts() {
  float raw = analogRead(BAT_V);
  float volts = 3.3*(2*raw)/1024;
  return volts;
}

uint8_t usb_power() {
  int raw_usbv_adc = analogRead(USB_V);
  if (raw_usbv_adc > 900) {
    return 1;
  }
  else {
    return 0;
  }
}

void setup() {
  // put your setup code here, to run once:

  vnRingBuffWriteHead = 0;
  prevTrigTime = 0;
  trigCount = 0;
  vnRingBuff[vnRingBuffWriteHead] = 0;

  speakerOn = SPEAKER_TOGGLE;

  // Enable interrupts

  // Settings for Timer0 interrupt
  OCR0A = 0xAF; // Count at which to insert interrupt
  TIMSK0 |= _BV(OCIE0A);
  // Attach geiger counter interrupt
  pinMode(GEIGER_TRG, INPUT_PULLUP);
  attachInterrupt(1, geigerEvent, FALLING);
  
  ledScreen = new LedScreen();
  speaker = new Speaker();
  quantumRand = new QuantumRNG(ledScreen, vnRingBuff, &vnRingBuffWriteHead);
  statusLed = new StatusLed();
  settingsDial = new SettingsDial(ledScreen);

  // Turn on Geiger board
  pinMode(GEIGER_PWR, OUTPUT);
  digitalWrite(GEIGER_PWR, HIGH);

  // Init UV LEDs
  pinMode(UV_LED_PIN, OUTPUT);

  // Burn in RNG
  quantumRand->burnIn(&trigCount);
}

void low_power_mode() {
  // Put system to sleep
  detachInterrupt(1);
  attachInterrupt(digitalPinToInterrupt(SETTINGS_BUTTON), wake_noop, FALLING);
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  digitalWrite(GEIGER_PWR, LOW);
  ledScreen->sleep();
  delay(500);
  sleep_cpu();

  // Wake it back up
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(SETTINGS_BUTTON));
  ledScreen->wake();
  attachInterrupt(1, geigerEvent, FALLING);
  digitalWrite(GEIGER_PWR, HIGH);
  trigCount = 0;
  quantumRand->burnIn(&trigCount);
}

void loop() {
  // put your main code here, to run repeatedly:

  uint8_t randB = quantumRand->getByte();
  
  char* displayDigits = LedScreen::number_to_digits(randB, 0);
  delay(2000);
  ledScreen->display(displayDigits);
  delay(2000);
  ledScreen->clear();

  delay(10);

  /*
  float bat_volts = read_bat_volts();
  int int_volts = (int)(1000*bat_volts);
  char* displayVal = LedScreen::number_to_digits(int_volts, 0);
  ledScreen->displayVolts(displayVal);
  delay(2000);
  ledScreen->clear();
  */

  if(settingsDial->buttonPushed()) {
    low_power_mode();
  }
}
