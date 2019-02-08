#include "unitoutf8.h"

String UnicodeUtf8::unicode2utf8(const String& uniStr)
{
    char utf8str[uniStr.length() + 1];
    char* p_utf8 = utf8str;
    const char* p_unicode = uniStr.c_str();
    while(*p_unicode) {
        if(p_unicode[0] == '\\' && p_unicode[1] == 'u') {
            uint16_t uv = 0;
            bool err = false;
            for(int i = 0; i < 4; i++) {
                uv <<= 4;
                char ch = p_unicode[2 + i];
                if(ch >= '0' && ch <= '9') {
                    uv += ch - '0';
                }
                else if(ch >= 'A' && ch <= 'F') {
                    uv += ch - 'A' + 10;
                }
                else if(ch >= 'a' && ch <= 'f') {
                    uv += ch - 'a' + 10;
                }
                else {
                    err = true;
                    break;
                }
            }
            if(err) {
                *(p_utf8++) = *(p_unicode++);
            }
            else {
                if(uv < 0x80) {
                    *(p_utf8++) = uv;
                }
                else if(uv < 0x800) {
                    *(p_utf8++) = 0xC0 | (uv >> 6);
                    *(p_utf8++) = 0x80 | (uv & 0x3F);
                }
                else {
                    *(p_utf8++) = 0xE0 | (uv >> 12);
                    *(p_utf8++) = 0x80 | ((uv >> 6) & 0x3F);
                    *(p_utf8++) = 0x80 | (uv & 0x3F);
                }
                p_unicode += 6;
            }
        }
        else {
            *(p_utf8++) = *(p_unicode++);
        }
    }
    *p_utf8 = 0x00;
    
    return utf8str;
}

uint16_t UnicodeUtf8::str2uint(const char* str, int max_len=4) {
    uint16_t rtn = 0;
    while(max_len--) {
        if(*str > 'F' || *str < '0') {
            break;
        }
        else {
            rtn <<= 4;
            rtn += *str - '0';
        }
    }
    return rtn;
}
