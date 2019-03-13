#ifndef _MY_RTC_H_
#define _MY_RTC_H_
#include <RtcDS3231.h>
#include <time.h>

#define RTC_GETTIME_TO_MS               (500)
#define REFRESH_SEC                     (3600)
#define RTC_CLOCK_INTERRUPT_PIN         17
#define DEF_LOCAL_TIMEZONE_MS           (8*3600)

extern RtcDateTime g_rtc_now;
extern volatile time_t g_time_now;

void rtc_setup();
void rtc_loop();
time_t convertRTC2Time(const RtcDateTime &rtc_datetime);
time_t localDateTime(const RtcDateTime &rtc_datetime, const int timezone_ms);
String getDateTimeStr(time_t time, const int timezone_ms );
String getCurrentRtcDTStr(const int timezone_ms=0);
time_t getRTCNow();

#endif  // _MY_RTC_H_