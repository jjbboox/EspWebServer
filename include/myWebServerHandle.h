#ifndef _MY_WEB_SERVER_HANDLE_H_
#define _MY_WEB_SERVER_HANDLE_H_
#include <ESPAsyncWebServer.h>

void myWebServerOnSetup();
void handleInit(AsyncWebServerRequest *request);
void handleSwitchLed(AsyncWebServerRequest *request);
void handleTranslate(AsyncWebServerRequest *request);
void handleGetTemperatuer(AsyncWebServerRequest *request);
void handleSetRGBColor(AsyncWebServerRequest *request);
void handleDHTLogs(AsyncWebServerRequest *request);
void handleGetPir(AsyncWebServerRequest *request);

#endif // _MY_WEB_SERVER_HANDLE_H_