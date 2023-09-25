#ifndef mqtt_h
#define mqtt_h

#include <PubSubClient.h>
#include "define_const.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "send_discord.h"
#include "helper.h"
#include "hum_temp.h"

extern PubSubClient mqttClient;

extern int userAPumpTime;
extern bool hasUserAPumpTime;

extern int userBPumpTime;
extern bool hasUserBPumpTime;

extern int userCPumpTime;
extern bool hasUserCPumpTime;

class MQTT {
  private:
    static void callback(char *topic, byte *payload, unsigned int length);
    void connect();
  public:
    MQTT();
    void init();
    bool reconnect();
};

#endif