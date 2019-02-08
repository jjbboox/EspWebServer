#ifndef _MY_RGB_LED_H_
#define _MY_RGB_LED_H_
#include <Arduino.h>

#define RED_PIN     D7
#define GREEN_PIN   D6
#define BLUE_PIN    D5


struct RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

void RGB_setup();
void setRGBColor(uint8_t red, uint8_t green, uint8_t blue);
void setRGBColor(const RGBColor&);
void RGB_loop();
RGBColor convertToColor(String strColor);

#endif  // _MY_RGB_LED_H_