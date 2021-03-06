// SETTINGS
#define SPEAKER_TOGGLE 0

#include <avr/sleep.h>

#include "display.h"
#include "speaker.h"
#include "QuantumRNG.h"
#include "StatusLed.h"
#include "SettingsDial.h"
#include "Keyswitch.h"

//
// Microcontroller pins
//

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define USB_V A3
#define BAT_V A6

#define KEYSWITCH_PIN 4

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
uint8_t trigLedsOn;
unsigned long int lastVoltageTime;
float averageVoltage;

LedScreen* ledScreen;
Speaker* speaker;
QuantumRNG* quantumRand;
StatusLed* statusLed;
SettingsDial* settingsDial;
Keyswitch* keyswitch;

//
// Interrupt functions
//

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

  speakerOn = 0;
  trigLedsOn = 0;
  lastVoltageTime = millis();

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
  keyswitch = new Keyswitch();
  settingsDial = new SettingsDial(ledScreen);

  pinMode(KEYSWITCH_PIN, INPUT_PULLUP);

  // Turn on Geiger board
  pinMode(GEIGER_PWR, OUTPUT);
  digitalWrite(GEIGER_PWR, HIGH);

  // Init UV LEDs
  pinMode(UV_LED_PIN, OUTPUT);

  statusLed->red();

  // Burn in RNG
  quantumRand->burnIn(&trigCount);

  averageVoltage = read_bat_volts();
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
  statusLed->red();
  trigCount = 0;
  quantumRand->burnIn(&trigCount);
}

void loop() {
  // put your main code here, to run repeatedly:

  /*
  if(keyswitch->pressed()) {
    uint8_t randB = quantumRand->getByte();
    
    char* displayDigits = LedScreen::number_to_digits(randB, 0);
    delay(2000);
    ledScreen->display(displayDigits);
    delay(2000);
    ledScreen->clear();
  }
  */

  // Update average voltage;
  unsigned long int currMs = millis();
  if((currMs - lastVoltageTime) > 1000) {
    lastVoltageTime = currMs;
    averageVoltage += (read_bat_volts() - averageVoltage) / (30.0 + 1.0);
  }

  if(averageVoltage < 3.4) {
    statusLed->blinkRed();
  }
  else {
    statusLed->red();
  }

  if(keyswitch->pressed()) {
    char rolls[] = {-1, -1, -1, -1};
    trigLedsOn = 1;
    speakerOn = 1;
    for(uint8_t i=0; i<settingsDial->getDiceNum(); i++) {
      rolls[i] = quantumRand->getDice(settingsDial->getDiceSize()) + 1;
    }
    trigLedsOn = 0;
    speakerOn = 0;
    ledScreen->displayRolls(rolls);
    while(1) {
      if(keyswitch->pressed()) {
        delay(200);
        break;
      }
    }
    ledScreen->clear();
  }

  /*
  if(keyswitch->pressed()) {
    statusLed->red();
    delay(1000);
  }
  else {
    statusLed->off();
  }
  */

  /*
  float bat_volts = read_bat_volts();
  int int_volts = (int)(1000*bat_volts);
  char* displayVal = LedScreen::number_to_digits(int_volts, 0);
  ledScreen->displayVolts(displayVal);
  delay(2000);
  ledScreen->clear();
  */
  if(settingsDial->buttonShortPressed()) {
    settingsDial->handleInput();
  }

  if(settingsDial->buttonLongPressed()) {
    statusLed->off();
    delay(1000);
    low_power_mode();
  }
}
