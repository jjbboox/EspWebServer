#ifndef _MY_PIR_H_
#define _MY_PIR_H_
#include <Arduino.h>

const int pir_pin = D4;     // 人体红外感应模块端口
#define HUMENACTIVEINFO_CNT     10
#define MERGE_SECS              (1 * 60)

struct HumenActiveInfo {
    time_t start_time;
    time_t end_time;
};

void pir_setup();
void pir_loop();
inline bool getPirStatus();
bool isHumenActive();
HumenActiveInfo* getPirLogs();
int getPirLogsLength();

#endif  // _MY_PIR_H_