#include "myDHT.h"
#include "myRTC.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"
#include <vector>

DHT dht(dht_pin, DHTTYPE);

float g_Temperature;
float g_Humidity;
float g_ComputeHeat;
std::vector<DHTLog> g_DHTLogs;

uint32_t dht_timeout = 0;
bool keepHumenActive = false;

void dht_setup()
{
    dht.begin();
    delay(20);
    g_Humidity = dht.readHumidity();
    g_Temperature = dht.readTemperature();
    g_ComputeHeat = dht.computeHeatIndex(g_Temperature, g_Humidity, false);
}

uint32_t g_dht_log_timeout = 0;

void dht_loop()
{
    // 更新当前传感器读数
    if (dht_timeout <= g_time_now)
    {
        dht_timeout = g_time_now + DHT_TIMEOUT_S;
        g_Humidity = dht.readHumidity();
        g_Temperature = dht.readTemperature();
        g_ComputeHeat = dht.computeHeatIndex(g_Temperature, g_Humidity, false);
    }
    
    // 记录传感器履历数据
    if(g_dht_log_timeout <= g_time_now) {
        g_dht_log_timeout = g_time_now + DHT_LOGS_TIMOUT_S;

        DHTLog newLog = {
            .log_time=g_time_now, 
            .Temperature = g_Temperature,
            .Humidity = g_Humidity,
            .ComputeHeat = g_ComputeHeat
        };
        dht_appendLog(newLog);
    }
}

float getHumidity()
{
    return g_Humidity;
}

float getTemperature()
{
    return g_Temperature;
}

float getComputeHeat()
{
    return g_ComputeHeat;
}

void dht_appendLog(const DHTLog& _log) {
    g_DHTLogs.insert(g_DHTLogs.begin(), _log);
    Serial.println("dht_appendLog excute");
    if(g_DHTLogs.size() > DHT_LOGS_SIZE) {
        Serial.printf("\tList Overflow do delete last.g_DHTLogs.size() = [%d]\r\n", g_DHTLogs.size());
        g_DHTLogs.erase(g_DHTLogs.end() - (g_DHTLogs.size() - DHT_LOGS_SIZE), g_DHTLogs.end());
    }
}