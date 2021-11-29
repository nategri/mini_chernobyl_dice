#ifndef PINS_H
#define PINS_H

// Toggle for whether to use 32U4
#define USE_32U4

//
// Microcontroller pins
//

#ifndef USE_32U4

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define USB_V A3
#define BAT_V A6

#define KEYSWITCH_PIN 4

#define SETTINGS_BUTTON 2
#define SETTINGS_RT1 9
#define SETTINGS_RT2 10

#define STATUS_GREEN A6
#define STATUS_RED 5

#define LED_DISP_DATAIN 11
#define LED_DISP_CLK 13
#define LED_DISP_LOAD A7

#define SPEAKER_PIN_A A1
#define SPEAKER_PIN_B A0

#else

#define GEIGER_PWR A2
#define GEIGER_TRG 3

#define UV_LED_PIN 6

#define USB_V A3
#define BAT_V A9

#define KEYSWITCH_PIN 13

#define SETTINGS_BUTTON 2
#define SETTINGS_RT1 9
#define SETTINGS_RT2 10

#define STATUS_GREEN A4
#define STATUS_RED 5

#define LED_DISP_DATAIN 16
#define LED_DISP_CLK 15
#define LED_DISP_LOAD A5

#define SPEAKER_PIN_A A1
#define SPEAKER_PIN_B A0

#endif

#endif
