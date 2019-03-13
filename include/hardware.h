#ifndef _MY_HARDWARE_H_
#define _MY_HARDWARE_H_
#include <Arduino.h>
#include "myPir.h"
#include "myRGBLed.h"
#include "MyDigitalSwitch.h"

// 板载LED定义
#ifdef  ESP32
#define LED_ON  HIGH
#define LED_OFF LOW
#elif defined(ESP8266)
#define LED_ON  LOW
#define LED_OFF HIGH
#endif

static const uint8_t Led_Pin   = LED_BUILTIN;
extern MyDigitalSwitch g_buildin_led;

// 继电器控制
// #ifdef  ESP32
// #define SWITCH_PIN      16
// #define SWITCH_PIN_ON   false
// #elif defined(ESP8266)
// #endif

// extern MyDigitalSwitch g_switch;

// 全彩LED定义
#ifdef  ESP32
    #define RED_PIN     27
    #define GREEN_PIN   26
    #define BLUE_PIN    25
#elif defined(ESP8266)
    #define RED_PIN     D7
    #define GREEN_PIN   D6
    #define BLUE_PIN    D5
#endif
extern RGBColorLED g_color;

// Pir
#ifdef ESP32
    const int pir_pin = 34;
#elif defined(ESP8266)
    const int pir_pin = D4;
#endif

extern MyPir g_pir;

void hw_setup();
void hw_loop();

#endif  //_MY_HARDWARE_H_
