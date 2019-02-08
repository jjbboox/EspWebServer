#include "myRGBLed.h"

RGBColor g_color;

void RGB_setup()
{
    setRGBColor(0, 0, 0);
}

void setRGBColor(uint8_t red, uint8_t green, uint8_t blue)
{
    g_color.red = red;
    g_color.green = green;
    g_color.blue = blue;
    analogWrite(RED_PIN, g_color.red);
    analogWrite(GREEN_PIN, g_color.green);
    analogWrite(BLUE_PIN, g_color.blue);
}

void setRGBColor(const RGBColor& color)
{
    setRGBColor(color.red, color.green, color.blue);
}

void RGB_loop()
{

}

RGBColor convertToColor(String strColor)
{
    strColor.toLowerCase();
    RGBColor color;
    color.green = color.red = color.blue = 0;
    if(strColor.startsWith("#")) {
        uint32_t ucolor = 0;
        for(uint16_t i = 1; i < strColor.length(); i++) {
            if((strColor[i] >= '0' &&  strColor[i] <= '9') || (strColor[i] >= 'a' && strColor[i] <= 'f')) {
                ucolor <<= 4;
                ucolor += (strColor[i] >='a' && strColor[i] <= 'f')?(10 + (strColor[i] - 'a')):(strColor[i]-'0');
            }
            else {
                break;
            }
        }
        color.red = (ucolor >> 16) & 0xff;
        color.green = (ucolor >> 8) & 0xff;
        color.blue = ucolor & 0xff;
    }
    return color;
}