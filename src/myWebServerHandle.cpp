#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "myWebServerHandle.h"
#include "hardware.h"
#include "myDHT.h"
#include "myPir.h"
#include "myRTC.h"
#include "myRGBLed.h"

extern ESP8266WebServer server;
extern String baiduTranslate(String q, String from, String to);
extern String toLanguage;
extern RGBColor g_color;

String getParam(const String &strline, const String &paramName);

void myWebServerOnSetup()
{
    server.on("/Init", HTTP_GET, handleInit);
    server.on("/Switch", HTTP_GET, handleSwitchLed);
    server.on("/Translate", HTTP_POST, handleTranslate);
    server.on("/getTemperature", HTTP_GET, handleGetTemperatuer);
    server.on("/SetRGBColor", HTTP_POST, handleSetRGBColor);
}

void handleSetRGBColor() {
    String PostData = "";
    if (server.hasArg("plain"))
    {
        PostData = server.arg("plain");
    }

    String strColor = getParam(PostData, "Color");

    RGBColor color = convertToColor(strColor);
    setRGBColor(color);
    Serial.println(strColor);
    server.send(200, "text/plain", "ok");
    
}

void handleGetTemperatuer()
{
    String jsonStr = "{";
    jsonStr += "\"temperature\":\"" + String(getTemperature()) + " ℃\",";
    jsonStr += "\"Humidity\":\"" + String(getHumidity()) + " %\",";
    jsonStr += "\"computeHeat\":\"" + String(getComputeHeat()) + " ℃\",";
    jsonStr += "\"HumenActive\":";
    jsonStr += "{";
    jsonStr += "\"status\":" + String(isHumenActive() ? "true" : "false") + ",";
    jsonStr += "\"list\":[";
    HumenActiveInfo *list = getPirLogs();
    for (int i = 0; i < getPirLogsLength(); i++)
    {
        if (list[i].start_time == 0 && list[i].end_time == 0)
        {
            break;
        }
        if (i > 0)
            jsonStr += ",";
        jsonStr += "{";
        jsonStr += String("\"start\":\"") + (list[i].start_time == 0 ? "" : getDateTimeStr(list[i].start_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
        jsonStr += String("\"end\":\"") + (list[i].end_time == 0 ? "" : getDateTimeStr(list[i].end_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
        jsonStr += String("\"mins\":\"");
        // 如果开始时间和终了时间均不为0则输出时间长度
        {
            if (list[i].start_time != 0 && list[i].end_time != 0)
            {
                time_t mins = list[i].end_time - list[i].start_time;
                jsonStr += String(mins / 3600) + ":" + String((mins % 3600) / 60) + "'" + String(mins % 60) + "\\\"";
            }
        }
        jsonStr += "\"";
        jsonStr += "}";
    }
    jsonStr += "]}";
    jsonStr += "}";
    Serial.println(jsonStr);
    server.send(200, "text/json", jsonStr);
}

void handleInit()
{
    // 将当前的状态数据组成JSON字符串返回给前台
    String strJson = "{ ";
    strJson += "\"ledstate\":";
    strJson += digitalRead(Led_Pin) == LOW?"\"on\"":"\"off\"";
    strJson += ", \"language\":\"" + toLanguage + "\"";
    char strColor[10];
    sprintf(strColor, "#%06x", (g_color.red << 16)+ (g_color.green<<8) + g_color.blue);
    strJson += ", \"RGBColor\":\"" + String(strColor) + "\"";
    strJson += "}";
    server.send(200, "text/json", strJson);
}

void handleSwitchLed()
{
    // 反转指示灯
    digitalWrite(Led_Pin, !digitalRead(Led_Pin));
    // 读取翻转后指示灯的状态，将状态发送给前台
    server.send(200, "text/plain", digitalRead(Led_Pin)==LOW?"on":"off");
}

void handleTranslate()
{
    String PostData = "";
    if (server.hasArg("plain"))
    {
        PostData = server.arg("plain");
    }

    toLanguage = getParam(PostData, "lang");
    if (toLanguage.equals(""))
    {
        toLanguage = "zh";
    }
    String srcText = getParam(PostData, "text");
    Serial.println("Source Text is" + srcText);

    srcText.trim();
    if (!srcText.equals(""))
    { // 如果翻译文本为空则不调用翻译API，直接返回空字符串
        // 调用翻译API，并返回翻译后的结果
        String targetText = baiduTranslate(srcText, "auto", toLanguage);
        server.send(200, "text/plain", targetText);
        // print(targetText);
        Serial.println(targetText);
    }
    else
    {
        server.send(200, "text/plain", "");
    }
}

String getParam(const String &strline, const String &paramName)
{
    String rtnStr = "";
    int startIndex = strline.indexOf(paramName + "=");
    if (startIndex == -1)
    {
        return "";
    }
    startIndex += paramName.length() + 1;
    int endIndex = strline.indexOf('&', startIndex);
    if (endIndex == -1)
    {
        rtnStr = strline.substring(startIndex);
    }
    else
    {
        rtnStr = strline.substring(startIndex, endIndex);
    }
    return rtnStr;
}
