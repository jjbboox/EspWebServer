#ifndef _MY_DHT_H_
#define _MY_DHT_H_
#include <Arduino.h>
#include <vector>

#define DHT_DEBUG
#ifdef ESP32
const int dht_pin = 32;     // 温湿度传感器端口
#elif defined(ESP8266)
const int dht_pin = D3;
#endif
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

#define DHT_LOGS_SIZE       24             // 履历最大件数
#define DHT_LOGS_TIMOUT_S   (60*60)        // 隔多少秒记录一次履历
#define DHT_TIMEOUT_S       (5)            // 隔多少秒更新一次传感器读数

struct DHTLog {
    time_t log_time;
    float Temperature;
    float Humidity;
    float ComputeHeat;
};

extern std::vector<DHTLog> g_DHTLogs;
extern float g_Temperature;
extern float g_Humidity;
extern float g_ComputeHeat;

float getHumidity();
float getTemperature();
float getComputeHeat();
void dht_setup();
void dht_loop();
void dht_appendLog(const DHTLog& _log);

#endif
