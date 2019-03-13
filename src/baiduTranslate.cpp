#include <Arduino.h>
#ifdef ESP32
    #include <HTTPClient.h>
#elif defined(ESP8266)
    #include <ESP8266HTTPClient.h>
#endif
#include <MD5Builder.h>
#include "urlencode.h"
#include <ArduinoJson.h>
#include "unitoutf8.h"
#include "baiduTranslate.h"

String toLanguage;

// String unicode2utf8(const String &uniStr);

String baiduTranslate(String q, String from = "auto", String to = "zh")
{
    MD5Builder md5;
    md5.begin();
    // q.replace("\n", "\\n");
    Serial.println(q);
    long salt = random(1000, 1000000);
    String strSalt = String(salt);
    
    String sign = appid + q + strSalt + apiKey;
    md5.add(sign);
    md5.calculate();
    String md5sign = md5.toString();
    String rtnStr;

#if defined(ESP8266) || defined(HTTP_REMOTE_GET)
    rtnStr = "{";
    rtnStr += "\"Type\":\"object\",";
    rtnStr += "\"Value\":{";
    rtnStr +=   "\"url\":\"" + host + "\",";
    rtnStr +=   "\"params\":{";
    // rtnStr +=       "\"q\":\"" + (q) + "\",";
    rtnStr +=       "\"from\":\"" + from + "\",";
    rtnStr +=       "\"to\":\"" + to + "\",";
    rtnStr +=       "\"appid\":\"" + appid + "\",";
    rtnStr +=       "\"salt\":\"" + strSalt + "\",";
    rtnStr +=       "\"sign\":\"" + md5sign + "\"";
    rtnStr +=     "}";
    rtnStr +=   "}";
    rtnStr += "}";

#else
    // StaticJsonBuffer<2048> baiduJsonBuffer;
    DynamicJsonBuffer baiduJsonBuffer;
    WiFiClient client;
    HTTPClient http;
    String url = host;
    url += "?q=" + urlencode(q) + "";
    url += "&from=" + from + "";
    url += "&to=" + to + "";
    url += "&appid=" + appid;
    url += "&salt=" + String(salt);
    url += "&sign=" + md5sign + "";
    
    if(!http.begin(client, url)) {
        return "***Error!http.begin failed";
    }
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
    // baiduJsonBuffer.clear();
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

    rtnStr = "{";
    rtnStr += "'Type':'string',";
    rtnStr += "'Value':'" + resultStr + "'";
    rtnStr += "}";
#endif
    return rtnStr;
}
