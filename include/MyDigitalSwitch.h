#ifndef _MY_DIGITAL_SWITCH_
#define _MY_DIGITAL_SWITCH_
#include <Arduino.h>

class MyDigitalSwitch {
    private:
        int     pin;
        bool    pinState;
        bool    userDefPinOn;
        void    setPinOff() {
            pinState = false;
            digitalWrite(pin, userDefPinOn?LOW:HIGH);
        };
        void    setPinOn() {
            pinState = true;
            digitalWrite(pin, userDefPinOn?HIGH:LOW);
        };
    public:
        MyDigitalSwitch(int _pin, bool _userOn, bool _state=false) : pin(_pin), pinState(_state), userDefPinOn(_userOn) {
            pinMode(pin, OUTPUT);
            setSwitch(false);
        };
        void setSwitch(bool _state) {
            if(_state) {
                setPinOn();
            }
            else {
                setPinOff();
            }
        };
        void toggleSwitch() {
            setSwitch(!pinState);
        };
        bool getSwitch() {
            return pinState;
        };
        String toString() {
            if(pinState) {
                return "on";
            }
            else {
                return "off";
            }
        };
};

#endif  // _MY_DIGITAL_SWITCH_