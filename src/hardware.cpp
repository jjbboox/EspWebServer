#include "hardware.h"
#include <Adafruit_Sensor.h>
#include "myDHT.h"
#include "myPir.h"
#include "myRTC.h"
#include "myRGBLed.h"

void hw_setup()
{
    pinMode(Led_Pin, OUTPUT);
    RGB_setup();
    
    rtc_setup();
    dht_setup();
    pir_setup();
}

void hw_loop()
{
    pir_loop();
    dht_loop();
    rtc_loop();
}