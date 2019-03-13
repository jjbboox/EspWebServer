#ifndef _MY_RGB_LED_H_
#define _MY_RGB_LED_H_
#include <Arduino.h>

#define BREATH_MS       (2000)
#define BREATH_HALF_MS  (BREATH_MS / 2)
#define BREATH_TICK_MS  (20)

struct RGBColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class RGBColorLED {
    public:
        RGBColorLED(int _redpin, int _greenpin, int _bluepin):RedPin(_redpin), GreenPin(_greenpin), BluePin(_bluepin){
            breath = false;
            breath_start = breath_timeout = 0;
        };
        void begin();
        void setColor(uint8_t _red, uint8_t _green, uint8_t _blue);
        void setColor(const RGBColor& _color);
        void setColor(const String& strColor);
        RGBColor getColor() {
            return Color;
        };
        String getHTMLColorStr();
        void show();
        void show(uint8_t _red, uint8_t _green, uint8_t _blue);
        void show(const RGBColor& _color);
        static RGBColor convertColor(const String& scolor);
        void loop();
        void setBreath(bool _flag) { breath = _flag;};
        bool getBreath() {return breath;}
    private:
        int RedPin, GreenPin, BluePin;
        bool breath;
        uint32_t breath_start;
        uint32_t breath_timeout;
        RGBColor Color;
};

#endif  // _MY_RGB_LED_H_