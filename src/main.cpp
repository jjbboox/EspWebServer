#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
// #include <ESP8266mDNS.h>
// #include "myHttpServer.h"
#include "webServer.h"
#include "hardware.h"
#include "myRTC.h"

extern String baiduTranslate(String q, String from, String to);

const char *ApName = "B.J_Weather";
IPAddress _ip = IPAddress(192, 168, 4, 1);
IPAddress _gw = IPAddress(192, 168, 4, 1);
IPAddress _sn = IPAddress(255, 255, 255, 0);

// MDNSResponder mdns;

void showHint();

void connectWifi()
{
  WiFiManager wifiManager;
  wifiManager.setAPStaticIPConfig(IPAddress(_ip), IPAddress(_gw), IPAddress(_sn));
  wifiManager.autoConnect(ApName);
}

void setup()
{
  Serial.begin(921600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  // 外部设备初始化
  hw_setup();

  connectWifi();
  while (WiFi.status() != WL_CONNECTED)
    ;
  // if(mdns.begin("myesp8266", WiFi.localIP()))
  // {
  //   Serial.println("MDNS is Responder.");
  // }
  Serial.println("WiFi Connected.");
  Serial.println("SSID: " + WiFi.SSID());
  Serial.println("IP address is " + WiFi.localIP().toString());
  Serial.println();
  showHint();
  // httpServerBegin();
  web_server_setup();
  // MDNS.addService("http","tcp", 80);
}

String inputText = "";
String toLanguage = "zh";

void loop()
{
  hw_loop();
  
  // rtc_loop();

  // httpServerLoopEvent();
  web_server_loop();
  
  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch != '\n' && ch != '\r')
    {
      inputText += ch;
    }
    else
    {
      Serial.println(inputText);
      if (inputText.startsWith("To:"))
      {
        toLanguage = inputText.substring(3);
      }
      else if (WiFi.status() == WL_CONNECTED)
      {
        Serial.print("\t");
        Serial.println(baiduTranslate(inputText, "auto", toLanguage));
        Serial.println();
      }
      else
      {
        Serial.println("***Error!WiFi is not Connected.");
      }
      inputText = "";
      showHint();
    }
  }
}

void showHint()
{
  Serial.println("current target language is " + toLanguage);
  Serial.println("(you can input like 'To:zh' to change target language)");
  Serial.println("Please input source to translate:");
}
