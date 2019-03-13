#include <Arduino.h>
#ifdef ESP32
#include <analogWrite.h>
#endif
#include "myRGBLed.h"

void RGBColorLED::begin()
{
#ifdef ESP32
    analogWriteResolution(RedPin, 12);
    analogWriteResolution(GreenPin, 12);
    analogWriteResolution(BluePin, 12);
#endif
    setColor(0, 0, 0);
}

void RGBColorLED::setColor(uint8_t _red, uint8_t _green, uint8_t _blue)
{
    Color.red = _red;
    Color.green = _green;
    Color.blue = _blue;
    show();
}

void RGBColorLED::setColor(const RGBColor& _color)
{
    setColor(_color.red, _color.green, _color.blue);
}

void RGBColorLED::setColor(const String& strColor)
{
    setColor(convertColor(strColor));
}

void RGBColorLED::show()
{
    if(!breath) {
        show(Color.red, Color.green, Color.blue);
    }
    else {
        breath_timeout = 0;
        breath_start = 0;
    }
}

void RGBColorLED::show(uint8_t _red, uint8_t _green, uint8_t _blue)
{
    analogWrite(RedPin, _red);
    analogWrite(GreenPin, _green);
    analogWrite(BluePin, _blue);
}

void RGBColorLED::show(const RGBColor& _color)
{
    show(_color.red, _color.green, _color.blue);
}

String RGBColorLED::getHTMLColorStr()
{
    uint32_t uColor;
    uColor = (Color.red << 16) + (Color.green << 8) + Color.blue;
    String rgbStr = "#";
    rgbStr += String(uColor, HEX);
    return rgbStr;
}

RGBColor RGBColorLED::convertColor(const String& scolor)
{
    String strColor = scolor;
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

void RGBColorLED::loop()
{
    if(breath) {
        uint32_t now = millis();
        if(breath_timeout < now) {
            breath_timeout = now + BREATH_TICK_MS;
            uint32_t breath_end = breath_start + BREATH_MS;
            
            if(breath_end < now ) {
                breath_start = now;
                breath_end = now + BREATH_MS;
            }
            
            RGBColor tmpColor;
            uint32_t breath_ms_now = now - breath_start;
            
            if(breath_ms_now > BREATH_HALF_MS) {
                breath_ms_now = breath_end - now;
            }
            tmpColor.red = Color.red * breath_ms_now / BREATH_HALF_MS;
            tmpColor.green = Color.green * breath_ms_now / BREATH_HALF_MS;
            tmpColor.blue = Color.blue * breath_ms_now / BREATH_HALF_MS;
            show(tmpColor);
        }
    }
    
}