#include <ESP8266WiFi.h>
#include "web_server.h"
#include "device_wifi.h"
// #include "web_socket.h"
#include "helper.h"
#include "mqtt.h"
#include "hum_temp.h"
#include "define_const.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "send_discord.h"
#include <ArduinoOTA.h>

WebServer webserver;
DeviceWifi deviceWifi;
// WebSocket websocket;
MQTT mqtt;
HumTemp humTemp(I2CADDRESS);

unsigned long milisecond;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 300000);

int rainSensorValue = 0;
bool isRaining = false;
int timeRaining = 0;

bool isReconnectMqtt = false;

int userAPumpTime = 0;
bool hasUserAPumpTime = false;

int userBPumpTime = 0;
bool hasUserBPumpTime = false;

int userCPumpTime = 0;
bool hasUserCPumpTime = false;

int previousSeconds;

void setup() {
  ESP.wdtDisable();
  ESP.wdtEnable(4000); 

  Serial.begin(115200); 

  pinMode(USERCPUMPPIN, OUTPUT);
  digitalWrite(USERCPUMPPIN, 0);

  pinMode(USERBPUMPPIN, OUTPUT);
  digitalWrite(USERBPUMPPIN, 0);

  pinMode(USERAPUMPPIN, OUTPUT);
  digitalWrite(USERAPUMPPIN, 0);

  deviceWifi.startWiFi();
  webserver.handler();

  // websocket.init();
  mqtt.init();
  
  timeClient.begin();

  new SendDiscord("Device started. Connected to wifi: "+WiFi.SSID()+", strength: "+String(dBmtoPercentage(WiFi.RSSI()))+"%");
  /**
   * Enable OTA update
   */
  ArduinoOTA.setPort(8266);
  ArduinoOTA.begin();
}

void loop() {
  ESP.wdtFeed();
  // put your main code here, to run repeatedly:
  if(WiFi.status() == WL_CONNECTED){
    // webSocket.loop();
    ArduinoOTA.handle();
    
    if(!mqttClient.connected()){
      digitalWrite(USERCPUMPPIN, 0);
      digitalWrite(USERAPUMPPIN, 0);
      digitalWrite(USERBPUMPPIN, 0);
      if(!isReconnectMqtt){
        isReconnectMqtt = true;
        new SendDiscord("Reconnect mqtt");
      }
      mqtt.reconnect();
    }
    else{
      if(isReconnectMqtt){
        isReconnectMqtt = false;        
        new SendDiscord("Connected mqtt");
      }
      mqttClient.loop();
    }

    timeClient.update();
  }
  else{
    digitalWrite(USERCPUMPPIN, 0);
    digitalWrite(USERAPUMPPIN, 0);
    digitalWrite(USERBPUMPPIN, 0);
    ESP.restart();
  }

  if(timeClient.getHours() == 17 && timeClient.getMinutes() == 10 && timeClient.getSeconds() == 0){
    new SendDiscord("Daily reboot at: "+String(timeClient.getFormattedTime()));
    ESP.restart();
  }

  int currentSeconds = timeClient.getSeconds();

  if(currentSeconds != previousSeconds){
    previousSeconds = currentSeconds;
    
    int *humtemp;

    if(hasUserAPumpTime){
      if(userAPumpTime > 0){
        userAPumpTime--;
      }
      else{
        digitalWrite(USERAPUMPPIN, 0);
        hasUserAPumpTime = false;
        userAPumpTime = 0;

        String userAStatus = String(digitalRead(USERAPUMPPIN));
        String pumpStatusPayload = "{\"userA\":"+userAStatus+"}";
        mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
      }
    }

    if(hasUserBPumpTime){
      if(userBPumpTime > 0){
        userBPumpTime--;
      }
      else{
        digitalWrite(USERBPUMPPIN, 0);
        hasUserBPumpTime = false;
        userBPumpTime = 0;

        String userBStatus = String(digitalRead(USERBPUMPPIN));
        String pumpStatusPayload = "{\"userB\":"+userBStatus+"}";
        mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
      }
    }

    if(hasUserCPumpTime){
      if(userCPumpTime > 0){
        userCPumpTime--;
      }
      else{
        digitalWrite(USERCPUMPPIN, 0);
        hasUserCPumpTime = false;
        userCPumpTime = 0;

        String userC = String(digitalRead(USERCPUMPPIN));
        String pumpStatusPayload = "{\"userC\":"+userC+"}";
        mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
      }
    }

    if(timeClient.getMinutes() == 0 || timeClient.getMinutes() == 30){
      if(timeClient.getSeconds() == 0){
        humtemp = humTemp.getHumTemp();

        String periodMessage = "Time: " + String(timeClient.getFormattedTime()) + ", nhiệt độ: " + String(humtemp[0]) + ", độ ẩm: " + String(humtemp[1]);
        SendDiscord *ds = new SendDiscord(periodMessage);
        ds->SendHumTemp(humtemp[1], humtemp[0]);
      }
    }

    rainSensorValue = analogRead(RAINSENSOR);

    if((timeClient.getSeconds() % 2 == 0)){
      humtemp = humTemp.getHumTemp();
      String payload = "{\"temp\":"+String(humtemp[0])+",\"hum\":"+String(humtemp[1])+" }";
      mqttClient.publish(EVENT_WEATHER, (char*) payload.c_str());

      String userAStatus = String(digitalRead(USERAPUMPPIN));
      String userBStatus = String(digitalRead(USERBPUMPPIN));
      String userC = String(digitalRead(USERCPUMPPIN));
      String pumpStatusPayload = "{\"userA\":"+userAStatus+",\"userB\":"+userBStatus+",\"userC\":"+userC+"}";
      mqttClient.publish(EVENT_MESSAGE, (char*) pumpStatusPayload.c_str());
    
      if(rainSensorValue <= 500){
        String weatherPayload = "{\"rain\": 1,\"value\": "+String(rainSensorValue)+"}";
        mqttClient.publish(EVENT_RAIN, (char*) weatherPayload.c_str());
      }
      else{
        String weatherPayload = "{\"rain\": 0}";
        mqttClient.publish(EVENT_RAIN, (char*) weatherPayload.c_str());
      }
    }

    if(rainSensorValue <= 460){
      timeRaining++;
      if(!isRaining && timeRaining >= 60){
        isRaining = true;
        new SendDiscord("Trời đang mưa");
      }
    }
    else{
      if(isRaining){
        isRaining = false;

        String strTimeRaining = secondsToTime(timeRaining);
        new SendDiscord("Trời tạnh mưa, thời gian mưa: "+strTimeRaining);
      }
      if(timeRaining > 0){
        timeRaining = 0;
      }
    }
  }
}
