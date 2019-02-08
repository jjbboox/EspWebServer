#include "myRTC.h"
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <time.h>

RtcDS3231<TwoWire> Rtc(Wire);
RtcDateTime g_rtc_now;
volatile time_t g_time_now;
volatile bool refreshflag = false;

void rtc_isr()
{
    g_time_now++;
    refreshflag = (g_time_now % REFRESH_SEC == 0);
}

void rtc_setup()
{
    pinMode(RTC_CLOCK_INTERRUPT_PIN, INPUT);
    Rtc.Begin();

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.Enable32kHzPin(false);
    // Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
    // Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);

    g_rtc_now = Rtc.GetDateTime();
    g_time_now = convertRTC2Time(g_rtc_now);
    char* dtStr = getDateTimeStr(g_time_now, 8 * 3600);
    Serial.println(dtStr);
    delay(20);
    // attachInterrupt(RTC_CLOCK_INTERRUPT_PIN, rtc_isr, RISING);
}

void rtc_loop()
{
    // // 如果更新标志为真，则重新从RTC中读取时间
    // if (refreshflag)
    // {
    //     g_rtc_now = Rtc.GetDateTime();
    //     g_time_now = convertRTC2Time(g_rtc_now);
    //     refreshflag = false;
    // }
}

time_t convertRTC2Time(const RtcDateTime &rtc_datetime)
{
    struct tm tmpTm;
    tmpTm.tm_sec = rtc_datetime.Second();
    tmpTm.tm_min = rtc_datetime.Minute();
    tmpTm.tm_hour = rtc_datetime.Hour();
    tmpTm.tm_mday = rtc_datetime.Day();
    tmpTm.tm_mon = rtc_datetime.Month() - 1;
    tmpTm.tm_year = rtc_datetime.Year() - 1970;
    return mktime(&tmpTm);
}

time_t localDateTime(const RtcDateTime &rtc_datetime, const int timezone_ms)
{
    return convertRTC2Time(rtc_datetime) + timezone_ms;
}

char* getDateTimeStr(time_t time, const int timezone_ms)
{
    time += timezone_ms;
    struct tm *tblock;
    tblock = localtime(&time);
    static char datetime[20];
    sprintf(datetime, "%04d-%02d-%02d %02d:%02d:%02d",
            tblock->tm_year + 1970, tblock->tm_mon + 1, tblock->tm_mday,
            tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
    return datetime;
}

char* getCurrentRtcDTStr(const int timezone_ms)
{
    time_t t = getRTCNow();
    return getDateTimeStr(t, timezone_ms);
}

time_t getRTCNow()
{
    RtcDateTime now = Rtc.GetDateTime();
    return convertRTC2Time(now);
}