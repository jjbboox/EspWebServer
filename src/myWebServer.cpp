#ifdef ESP32
  #include "WiFi.h"
  #include "SPIFFS.h"
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include "FS.h"
#endif
#include <ESPAsyncWebServer.h>
#include "myWebServerHandle.h"

const char* ssid = "xxxxxxxx";
const char* password =  "xxxxxxxxxxx";

AsyncWebServer server(80);

String getContentType(String filename) {
  if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  } else if (filename.endsWith(".svg")) {
    return "image/svg+xml";
  }
            
  return "text/plain";
}


void web_server_setup(){
  Serial.begin(921600);

  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  // 主页
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  // 用户自定义
  myWebServerOnSetup();

  // 未找到
  server.onNotFound([](AsyncWebServerRequest *request) {
      String path = request->url();
      Serial.println("WebServer.onNotFound:[" + path + "]");
      Serial.println("\tHeader:[" + String(request->methodToString()) + "]");
      
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
          Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
          Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
          Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      
      String contentType = getContentType(path); 
      request->send(SPIFFS, path, contentType);
  });
  server.begin();
}

void web_server_loop(){}
