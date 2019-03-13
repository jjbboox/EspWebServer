#include "hardware.h"
#include <Adafruit_Sensor.h>
#include "myDHT.h"
#include "myPir.h"
#include "myRTC.h"
#include "myRGBLed.h"
#include "MyDigitalSwitch.h"

// RGB LED
RGBColorLED g_color(RED_PIN, GREEN_PIN, BLUE_PIN);
MyPir g_pir(pir_pin);
// MyDigitalSwitch g_switch(SWITCH_PIN, SWITCH_PIN_ON);
MyDigitalSwitch g_buildin_led(Led_Pin, LED_ON);

// 硬件外设初始化
void hw_setup()
{
    // 板载LED灯初始化
    // pinMode(Led_Pin, OUTPUT);
    // digitalWrite(Led_Pin, LED_OFF);
    
    // RGBLED灯初始化
    g_color.setBreath(true);
    g_color.begin();
    
    // RTC初始化
    rtc_setup();

    // 温湿度计初始化
    dht_setup();

    // 人体感应器初始化
    g_pir.begin();
    // 设置最大记录数
    g_pir.setMaxLogs(10);
}

// 硬件外设Loop处理
void hw_loop()
{
    g_color.loop();
    g_pir.loop();
    rtc_loop();
    dht_loop();
}