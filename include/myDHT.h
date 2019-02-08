#ifndef _MY_DHT_H_
#define _MY_DHT_H_
#include <Arduino.h>

const int dht_pin = D3;     // 温湿度传感器端口
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

extern float g_Temperature;
extern float g_Humidity;
extern float g_ComputeHeat;

#define DHT_TIMEOUT_MS  (5000)

float getHumidity();
float getTemperature();
float getComputeHeat();
void dht_setup();
void dht_loop();

#endif
