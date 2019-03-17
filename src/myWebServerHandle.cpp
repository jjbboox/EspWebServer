#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "myWebServerHandle.h"
#include "hardware.h"
#include "myDHT.h"
#include "myPir.h"
#include "myRTC.h"
#include "myRGBLed.h"
#include "MyDigitalSwitch.h"

extern AsyncWebServer server;
extern String baiduTranslate(String q, String from, String to);
extern String toLanguage;

const String& getPostParam(AsyncWebServerRequest* request, const String& paramName);

void myWebServerOnSetup()
{
    server.on("/Init", HTTP_GET, handleInit);
    server.on("/Switch", HTTP_GET, handleSwitchLed);
    server.on("/getTemperature", HTTP_GET, handleGetTemperatuer);
    server.on("/getPir", HTTP_GET, handleGetPir);
    server.on("/DHTLogs", HTTP_GET, handleDHTLogs);
    server.on("/SetRGBColor", HTTP_POST, handleSetRGBColor);
    server.on("/Translate", HTTP_POST, handleTranslate);
    server.on("/HeWeather", HTTP_GET, [](AsyncWebServerRequest* request){
        // 获取和风天气的APIKey
        request->send(200, "text/plain", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
    });
}

void handleSetRGBColor(AsyncWebServerRequest *request) {
    Serial.println("on.RGBColor");

    const String& isBreath = getPostParam(request, "isBreath");
    if(!isBreath.equals("")) {
        g_color.setBreath(isBreath.equalsIgnoreCase("true")?true:false);
    }

    const String& strColor = getPostParam(request, "Color");
    if(!strColor.equals("")) {
        g_color.setColor(strColor);
    }

    if(isBreath.equals("") && strColor.equals("")) {
        request->send(400, "text/plain", "error");
    }
    else {
        request->send(200, "text/plain", "ok");
    }
}

void handleDHTLogs(AsyncWebServerRequest *request)
{
    Serial.println("on.DHTLogs");
    String jsonStr = "{";
    jsonStr += "\"DHTLogs\":[";
    String logsStr = "";
    for (const DHTLog hlog : g_DHTLogs)
    {
        if (!logsStr.equals(""))
            logsStr += ",";
        
        logsStr += "{";
        logsStr += String("\"Hour\":\"") + getDateTimeStr(hlog.log_time, DEF_LOCAL_TIMEZONE_MS) + "\",";
        logsStr += String("\"temperature\":\"") + hlog.Temperature + "\",";
        logsStr += String("\"Humidity\":\"") + hlog.Humidity + "\",";
        logsStr += String("\"computeHeat\":\"") + hlog.ComputeHeat + "\"";
        logsStr += "}";
    }
    jsonStr += logsStr;
    jsonStr += "]";
    jsonStr += "}";
    Serial.println(jsonStr);
    request->send(200, "text/json", jsonStr);
}

void handleGetPir(AsyncWebServerRequest *request)
{
    Serial.println("on.getPir");
    String jsonStr = "{";
    jsonStr += "\"HumenActive\":";
    jsonStr += "{";
    jsonStr += "\"status\":" + String(g_pir.isHumenActive() ? "true" : "false") + ",";
    jsonStr += "\"list\":[";
    String logsStr = "";
    for (const HumenActiveInfo hlog : g_pir.getPirLogs())
    {
        if (hlog.start_time == 0 && hlog.end_time == 0)
        {
            break;
        }
        if (!logsStr.equals(""))
            logsStr += ",";
        
        logsStr += "{";
        logsStr += String("\"start\":\"") + String(hlog.start_time == 0 ?"":getDateTimeStr(hlog.start_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
        logsStr += String("\"end\":\"") + String(hlog.end_time == 0 ? "" : getDateTimeStr(hlog.end_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
        logsStr += String("\"mins\":\"");
        // 如果开始时间和终了时间均不为0则输出时间长度
        {
            if (hlog.start_time != 0 && hlog.end_time != 0)
            {
                time_t mins = hlog.end_time - hlog.start_time;
                logsStr += String(mins / 3600) + ":" + String((mins % 3600) / 60) + "'" + String(mins % 60) + "\\\"";
            }
        }
        logsStr += "\"";
        logsStr += "}";
    }
    jsonStr += logsStr;
    jsonStr += "]}";
    jsonStr += "}";
    Serial.println(jsonStr);
    request->send(200, "text/json", jsonStr);
}

void handleGetTemperatuer(AsyncWebServerRequest *request)
{
    Serial.println("on.Temperatuer");
    String jsonStr = "{";
    jsonStr += "\"temperature\":\"" + String(getTemperature()) + " ℃\",";
    jsonStr += "\"Humidity\":\"" + String(getHumidity()) + " %\",";
    jsonStr += "\"computeHeat\":\"" + String(getComputeHeat()) + " ℃\"";
    jsonStr += "}";
    request->send(200, "text/json", jsonStr);
}

void handleInit(AsyncWebServerRequest *request)
{
    Serial.println("on.Init");
    // 将当前的状态数据组成JSON字符串返回给前台
    String strJson = "{ ";
    strJson += "\"ledstate\":";
    strJson += digitalRead(Led_Pin) == LED_ON?"\"on\"":"\"off\"";
    strJson += ",\"language\":\"" + toLanguage + "\"";
    strJson += ",\"RGBColor\":\"" + g_color.getHTMLColorStr() + "\"";
    strJson += ",\"RGBBreath\":" + (g_color.getBreath()?String("true"):String("false"));
    strJson += "}";
    request->send(200, "text/json", strJson);
    Serial.println(strJson);
}

void handleSwitchLed(AsyncWebServerRequest *request)
{
    Serial.println("on.SwitchLed");
    // 反转指示灯
    g_buildin_led.toggleSwitch();
    // digitalWrite(Led_Pin, !digitalRead(Led_Pin));
    // 读取翻转后指示灯的状态，将状态发送给前台
    // request->send(200, "text/plain", digitalRead(Led_Pin)==LED_ON?"on":"off");
    request->send(200, "text/plain", g_buildin_led.toString());
}

const String& getPostParam(AsyncWebServerRequest* request, const String& paramName)
{
    const static String errorStr = "";
    AsyncWebParameter* awp = request->getParam(paramName, true);
    if(awp == nullptr) {
        return errorStr;
    }
    else {
        return awp->value();
    }
}

void handleTranslate(AsyncWebServerRequest *request)
{
    Serial.println("on.Translate");
    const String& paramLang = getPostParam(request, "lang");
    const String& paramText = getPostParam(request, "text");

    if(paramLang.equals("")) {
        toLanguage = "zh";
    }
    else {
        toLanguage = paramLang;
    }

    if(paramText.equals("")) {
        request->send(400, "text/plain", "error");
    }
    else {
        String targetText = baiduTranslate(paramText, "auto", toLanguage);
        Serial.println(targetText);
        request->send(200, "text/json", targetText);
    }
}
