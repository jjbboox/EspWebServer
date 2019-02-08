#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <MD5Builder.h>
#include "urlencode.h"
#include <ArduinoJson.h>
#include "unitoutf8.h"

const char *targetLanguage = "zh";
const String host = "http://api.fanyi.baidu.com/api/trans/vip/translate";
const String appid = "20190120000257962";
const String apiKey = "a7YX_Rr5O_aJxyD1GWWG";

StaticJsonBuffer<2048> baiduJsonBuffer;

String unicode2utf8(const String &uniStr);

String baiduTranslate(String q, String from = "auto", String to = "zh")
{
    HTTPClient http;
    MD5Builder md5;
    md5.begin();
    long salt = random(1000, 1000000);
    String sign = appid + q + String(salt) + apiKey;
    md5.add(sign);
    md5.calculate();
    String md5sign = md5.toString();
    String url = host;
    url += "?q=" + urlencode(q) + "";
    url += "&from=" + from + "";
    url += "&to=" + to + "";
    url += "&appid=" + appid;
    url += "&salt=" + String(salt);
    url += "&sign=" + md5sign + "";
    http.begin(url);
    int httpCode = http.GET();
    String response = "";
    if (httpCode > 0)
    {
        if (httpCode == HTTP_CODE_OK)
        {
            response = http.getString();
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    response = UnicodeUtf8::unicode2utf8(response);
    baiduJsonBuffer.clear();
    JsonObject &root = baiduJsonBuffer.parseObject(response);
    if (!root.success())
    {
        return "***Error!parseObject() failed";
    }

    JsonArray &results = root["trans_result"].as<JsonArray>();
    String resultStr = "";
    for (unsigned int i = 0; i < results.size(); i++)
    {
        String rs = results[i]["dst"].as<String>();
        resultStr += rs + "\n";
    }

    return resultStr;
}
