#ifndef web_server_h
#define web_server_h

#include "define_const.h"
#include <ESPAsyncWebServer.h>
#include <AsyncHttpClient.h>
#include "mqtt.h"
#include <WiFiUdp.h>
#include <WakeOnLan.h>

class WebServer {
  public:
    WebServer();
    void handler();
};
#endif