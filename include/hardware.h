#ifndef _MY_HARDWARE_H_
#define _MY_HARDWARE_H_
#include <Arduino.h>

static const uint8_t Led_Pin   = LED_BUILTIN;

void hw_setup();
void hw_loop();

#endif  //_MY_HARDWARE_H_
