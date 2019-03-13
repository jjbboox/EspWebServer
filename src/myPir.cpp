#include "myPir.h"
#include "myRTC.h"
#include <time.h>

void MyPir::begin()
{
    pinMode(pirPin, INPUT);
    logs.clear();
}

void MyPir::begin(int _pin)
{
    pirPin = _pin;
    begin();
}

bool MyPir::getPirStatus()
{
    return digitalRead(pirPin);
}

void MyPir::loop()
{
    int currentState = digitalRead(pirPin);
    if(currentState != keepState) {
        keepState = currentState;
        time_t now = getRTCNow();
        
        // 上跳沿
        if(currentState == HIGH) {
            if(!logs.empty() && now - logs.front().end_time <= marginSecs) {
                logs.front().end_time = 0;
            }
            else {
                HumenActiveInfo newLog;
                newLog.start_time = now;
                newLog.end_time = 0;
                logs.insert(logs.begin(), newLog);
                if(logMaxSize > 0 && logs.size() > logMaxSize) {
                    logs.erase(logs.end() - (logs.size() - logMaxSize), logs.end());
                }
            }
        }
        // 下降沿
        else {
            logs.front().end_time = now;
        }
    }
}

bool MyPir::isHumenActive()
{
    return keepState == HIGH;
}

