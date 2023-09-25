#include "send_discord.h"

AsyncHttpClient asyncClient;
AsyncHttpClient asyncClient2;

SendDiscord::SendDiscord(String message){
  Serial.println("-------------------------------------");
  Serial.println(message);
  Serial.println("-------------------------------------");
  String postData = "{\"content\":\"" + message + "\"}";
  asyncClient.init("POST", DISCORDURL, "application/json", postData);
  asyncClient.send();
}

void SendDiscord::SendHumTemp(int hum, int temp){
  Serial.println("-------------------------------------");
  Serial.println("Hum temp log");
  Serial.println("-------------------------------------");
  String postData = "{\"humidity\":\""+String(hum)+"\",\"temperature\":\""+String(temp)+"\"}";
  asyncClient2.init("POST", HUMTEMPURL, "application/json", postData);
  asyncClient2.send();
}