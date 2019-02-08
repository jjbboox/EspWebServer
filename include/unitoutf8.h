#ifndef _UNICODE_TO_UTF8_H_
#define _UNICODE_TO_UTF8_H_
#include <Arduino.h>

class UnicodeUtf8
{
    public:
        static String unicode2utf8(const String& uniStr);
    private:
        static uint16_t str2uint(const char* str, int max_len);
};

#endif  // _UNICODE_TO_UTF8_H_