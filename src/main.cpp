#include <Arduino.h>
#include "hardware.h"
#include "myWebServer.h"

void setup() {
  // setup web server
  hw_setup();
  web_server_setup();
}

void loop() {
  // TODO:
  hw_loop();
}