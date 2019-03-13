#ifndef _MY_PIR_H_
#define _MY_PIR_H_
#include <Arduino.h>
#include <vector>

struct HumenActiveInfo {
    time_t start_time;
    time_t end_time;
};

class MyPir {
    public:
        MyPir():pirPin(0), keepState(LOW), logMaxSize(10), marginSecs(60) { logs.clear(); };
        MyPir(int _pin, int _max_logs=10, int _margin_secs = 60):pirPin(_pin), keepState(LOW), logMaxSize(_max_logs), marginSecs(_margin_secs) {};
        void begin();
        void begin(int);
        void loop();
        bool isHumenActive();
        bool getPirStatus();
        size_t getLogSize();
        const std::vector<HumenActiveInfo>& getPirLogs() { return logs; };
        int getMarginSecs() { return marginSecs;};
        void setMarginSecs(int _secs) { marginSecs = _secs;};
        int getMaxLogs() { return logMaxSize; };
        void setMaxLogs(int _max_logs) { logMaxSize = _max_logs; };
    private:
        int pirPin;
        int keepState;
        int logMaxSize;
        int marginSecs;
        std::vector<HumenActiveInfo> logs;
};

#endif  // _MY_PIR_H_