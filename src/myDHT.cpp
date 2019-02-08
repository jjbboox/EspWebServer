#include "myDHT.h"
#include <Adafruit_Sensor.h>
#include "DHT.h"

DHT dht(dht_pin, DHTTYPE);

float g_Temperature;
float g_Humidity;
float g_ComputeHeat;

uint32_t dht_timeout = 0;
bool keepHumenActive = false;

void dht_setup()
{
    dht.begin();
}

void dht_loop()
{
    if (dht_timeout < millis())
    {
        dht_timeout = millis() + DHT_TIMEOUT_MS;
        g_Humidity = dht.readHumidity();
        g_Temperature = dht.readTemperature();
        g_ComputeHeat = dht.computeHeatIndex(g_Temperature, g_Humidity, false);
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
