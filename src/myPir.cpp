#include "myPir.h"
#include "myRTC.h"
#include <time.h>

bool g_Pir = false;
int g_PirCount = 0;

HumenActiveInfo hais[HUMENACTIVEINFO_CNT] = {{0, 0}};

void pir_setup()
{
    pinMode(pir_pin, INPUT);
    for(int i = 0; i < HUMENACTIVEINFO_CNT; i++) {
        hais[i].start_time = hais[i].end_time = 0;
    }
    g_Pir = getPirStatus();
    if(g_Pir) {
        hais[0].start_time = getRTCNow();
        // // test code
        // hais[1].end_time = getRTCNow() + 3758;
        // hais[1].start_time = getRTCNow();
    }
}

void pir_loop()
{
    // 如果检测到人员开始活动
    bool currentStatus = getPirStatus();
    if(currentStatus != g_Pir) {
        // Serial.print("pir status change(g_Pir, currentStatus):");
        // Serial.print(g_Pir);
        // Serial.print(",");
        // Serial.println(currentStatus);
        g_Pir = currentStatus;
        time_t tmpTime = getRTCNow();
        if(g_Pir) {
            if((tmpTime - hais[0].end_time) <= MERGE_SECS) {
                hais[0].end_time = 0;
            }
            else {
                for(int i = HUMENACTIVEINFO_CNT - 1; i > 0; i--) {
                    hais[i] = hais[i-1];
                }
                hais[0].start_time = tmpTime;
                hais[0].end_time = 0;
            }
        }
        else {
            hais[0].end_time = tmpTime;
        }
        // for(int i = 0; i < HUMENACTIVEINFO_CNT; i++) {
        //     Serial.printf("hais[%d]=(%ul, %ul)\r\n", i, hais[i].start_time, hais[i].end_time);
        // }
    }
}

inline bool getPirStatus()
{
    return digitalRead(pir_pin) == HIGH;
}

bool isHumenActive()
{
    return getPirStatus();
}

HumenActiveInfo* getPirLogs() {
    return hais;
}

int getPirLogsLength()
{
    return HUMENACTIVEINFO_CNT;
}