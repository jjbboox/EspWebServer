#include <ESP8266WiFi.h>
#include <FS.h>
#include "myHttpServer.h"
#include "hardware.h"
#include "myDHT.h"
#include "myPir.h"
#include "myRTC.h"

WiFiServer server(80);
extern String toLanguage;
extern String baiduTranslate(String q, String from, String to);
String urldecode(String str);

String readString = ""; // 建立一个字符串对象用来接收存放来自客户的数据
String getParam(const String &strline, const String &paramName);
String getPostData(WiFiClient &client);
bool httpSendFile(WiFiClient &client, String fileName, String contentType);
String getSplitString(const String &str, const char Separator, uint16_t strIndex);
String getAfterKey(const String& fullStr, const String& key);

// 响应头
String responseHeaders =
    String("") +
    "HTTP/1.1 200 OK\r\n" +
    "Content-Type: text/html\r\n" +
    "Connection: close\r\n" +
    "\r\n";
String responseHeadersReturn = "HTTP/1.1 200 OK\r\n";
String responseHeadersContentType = "Content-Type: {$type}\r\n";
String responseHeadersConnectionClose = "Connection: close\r\n\r\n";

bool humen_flag = false;


void httpServerBegin()
{
    server.begin();
    
    Serial.println("Http Server is begin.");
}

void httpServerLoopEvent()
{
    WiFiClient client = server.available(); // 尝试建立客户对象
    if (client)                             // 如果当前有客户可用
    {
        boolean currentLineIsBlank = true;
        Serial.println("[Client connected]");

        while (client.connected()) // 如果客户端建立连接
        {
            if (client.available()) // 等待有可读数据
            {
                char c = client.read(); // 读取一字节数据
                readString += c;        // 拼接数据

                // 相应客户端的各种请求内容
                if (c == '\n' && currentLineIsBlank) // 等待请求头接收完成(接收到空行)
                {
                    String file_name = getSplitString(readString, ' ', 1);
                    String contentType = getAfterKey(readString, "Accept:");
                    contentType.trim();
                    if(contentType.equals("")) {
                        contentType = "text/html";
                    }

                    file_name = getSplitString(file_name, '?', 0);
                    if (httpSendFile(client, file_name, contentType))
                    {
                        // nothing to do here
                    }
                    // 如果不是SPIFFS中已存在的文件
                    else if (readString.startsWith("GET / HTTP/1.1")) // 如果是网页请求
                    {
                        // 从SPIFFS中读取/index.html文件发送到用户浏览器
                        SPIFFS.begin();
                        if (SPIFFS.exists("/index.html"))
                        {
                            File f = SPIFFS.open("/index.html", "r");
                            if (f)
                            {
                                client.print(responseHeaders);
                                while (f.available())
                                {
                                    client.write(f.read());
                                }
                            }
                            f.close();
                        }
                        SPIFFS.end();
                    }
                    else if (readString.startsWith("GET /getTemperature")) // 如果是改变LED状态请求
                    {
                        String jsonStr = "{";
                        jsonStr += "\"temperature\":\"" + String(getTemperature()) + " ℃\",";
                        jsonStr += "\"Humidity\":\"" + String(getHumidity()) + " %\",";
                        jsonStr += "\"computeHeat\":\"" + String(getComputeHeat()) + " ℃\",";
                        jsonStr += "\"HumenActive\":";
                        jsonStr += "{";
                        jsonStr += "\"status\":" + String(isHumenActive()?"true":"false") + ",";
                        jsonStr += "\"list\":[";
                        HumenActiveInfo* list = getPirLogs();
                        for(int i = 0; i < getPirLogsLength(); i++) {
                            if(list[i].start_time == 0 && list[i].end_time == 0) {
                                break;
                            }
                            if(i > 0) jsonStr += ",";
                            jsonStr += "{";
                            jsonStr += String("\"start\":\"") + (list[i].start_time == 0?"":getDateTimeStr(list[i].start_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
                            jsonStr += String("\"end\":\"") + (list[i].end_time == 0?"":getDateTimeStr(list[i].end_time, DEF_LOCAL_TIMEZONE_MS)) + "\",";
                            jsonStr += String("\"mins\":\"");
                            if(list[i].start_time == 0 || list[i].end_time == 0) {
                                jsonStr += "\"";
                            }
                            else {
                                time_t mins = list[i].end_time - list[i].start_time;
                                jsonStr += String(mins / 3600) + ":" + String((mins % 3600) / 60) + "'" + String(mins % 60) + "\\\"\"";
                            }
                            jsonStr += "}";
                        }
                        jsonStr += "]}";
                        jsonStr += "}";
                        Serial.println(jsonStr);
                        client.print(jsonStr);
                    }
                    else if (readString.startsWith("GET /Switch")) // 如果是改变LED状态请求
                    {
                        // 反转指示灯
                        digitalWrite(Led_Pin, !digitalRead(Led_Pin));
                        // 读取翻转后指示灯的状态，将状态发送给前台
                        if (digitalRead(Led_Pin) == LOW)
                        {
                            client.print("on");
                        }
                        else
                        {
                            client.print("off");
                        }
                    }
                    else if (readString.startsWith("GET /Init")) // 如果是初始化请求
                    {
                        // 将当前的状态数据组成JSON字符串返回给前台
                        String strJson = "{ ";
                        strJson += "\"ledstate\":";
                        if (digitalRead(Led_Pin) == LOW)
                        {
                            strJson += "\"on\"";
                        }
                        else
                        {
                            strJson += "\"off\"";
                        }
                        strJson += ", \"language\":\"" + toLanguage + "\"}";
                        Serial.println(strJson);
                        client.print(strJson);
                    }
                    else if (readString.startsWith("POST /Translate")) // 如果是翻译请求
                    {
                        // 从POST内容中获取目标语言的种类和翻译的原文
                        String PostData = getPostData(client);
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
                            client.print(targetText);
                            Serial.println(targetText);
                        }
                        else
                        {
                            client.print("");
                        }
                    }
                    else
                    {
                        client.print("\r\n");
                    }
                    break;
                }

                if (c == '\n')
                {
                    currentLineIsBlank = true; // 开始新行
                }
                else if (c != '\r')
                {
                    currentLineIsBlank = false; // 正在接收某行中
                }
            }
        }
        delay(1);      // 等待客户完成接收
        client.stop(); // 结束当前连接:
        Serial.println("[Client disconnected]");

        Serial.println(readString); // 打印输出来自客户的数据
        readString = "";
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

String getPostData(WiFiClient &client)
{
    String rtnStr = "";
    while (client.available())
    {
        char c = client.read();
        rtnStr += c;
    }
    return rtnStr;
}

String getSplitString(const String &str, const char Separator, uint16_t strIndex)
{
    String rtnStr = "";
    int start_index = 0;
    uint16_t count = 0;
    while (count++ < strIndex)
    {
        start_index = str.indexOf(Separator, start_index);
        if (start_index == -1)
        {
            return "";
        }
        for (; str[start_index] == Separator; start_index++)
            ;
    }
    rtnStr = str.substring(start_index, str.indexOf(Separator, start_index));
    return rtnStr;
}

bool httpSendFile(WiFiClient &client, String fileName, String contentType)
{
    bool isExists = false;
    SPIFFS.begin();
    if (SPIFFS.exists(fileName))
    {
        File f = SPIFFS.open(fileName, "r");
        if (f)
        {
            // String contentStr = responseHeadersContentType;
            // if(fileName.indexOf(".css") != -1) {
            //     contentStr.replace("{$type}", "text/css");
            // }
            // else if(fileName.indexOf(".png") != -1) {
            //     contentStr.replace("{$type}", "image/png");
            // }
            // else if(fileName.indexOf(".ico") != -1) {
            //     contentStr.replace("{$type}", "image/x-icon");
            // }
            // // contentStr.replace("{$type}", contentType);
            // String header = responseHeadersReturn + 
            //     contentStr +
            //     responseHeadersConnectionClose;
            // Serial.println("Response Header: " + header);
            // client.print(header);
            while (f.available())
            {
                client.write(f.read());
            }
            isExists = true;
        }
        f.close();
    }
    else
        SPIFFS.end();
    return isExists;
}

String getAfterKey(const String& fullStr, const String& key) {
    int index = fullStr.indexOf(key);
    int toIndex = -1;
    if(index == -1) {
        return "";
    }
    index += key.length();
    toIndex = fullStr.indexOf('\r', index);
    if(toIndex == -1) {
        toIndex = fullStr.indexOf('\n', index);
    }
    return fullStr.substring(index, toIndex);
}