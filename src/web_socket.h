#ifndef web_socket_h
#define web_socket_h

#include "define_const.h"
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include "send_discord.h"

extern WebSocketsServer webSocket;

class WebSocket {
  private:
    static void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
    static void receiveCommand(uint8_t num, uint8_t * payload);
  public:
    WebSocket();
    void init();
};

#endif