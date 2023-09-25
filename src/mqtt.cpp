#include "mqtt.h"
WiFiClient espClient;
PubSubClient mqttClient(espClient);
DynamicJsonDocument mqttDoc(1024);
HumTemp humTempTest(I2CADDRESS);

String client_id = "esp8266-client";

MQTT::MQTT(){

}

void MQTT::connect(){
  mqttClient.setCallback(callback);
  mqttClient.publish(EVENT_MESSAGE, "hello emqx");
  mqttClient.subscribe(EVENT_PUMP_CONTROL);
}

void MQTT::init(){
  mqttClient.setKeepAlive(90);
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  if(mqttClient.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)){
    this->connect();
  };
}

bool MQTT::reconnect(){
  if(!mqttClient.connected()){
    if(mqttClient.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)){
      this->connect();
      return true;
    }
    return false;
  }
  return true;
}

void MQTT::callback(char *topic, byte *payload, unsigned int length){
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");

  DeserializationError error = deserializeJson(mqttDoc, payload);
  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  if(!strcmp(topic, EVENT_PUMP_CONTROL)){
    bool hasUserA = !mqttDoc["userA"].isNull();
    bool hasUserB = !mqttDoc["userB"].isNull();
    bool hasUserC = !mqttDoc["userC"].isNull();
    bool hasTest = !mqttDoc["test"].isNull();
    bool hasReset = !mqttDoc["reset"].isNull();

    String userAStatus = String(digitalRead(USERAPUMPPIN));
    String userBStatus = String(digitalRead(USERBPUMPPIN));
    String userCStatus = String(digitalRead(USERCPUMPPIN));
    
    if(hasTest){
      int *humtemp = humTempTest.getHumTemp();
      int temp = humtemp[0];
      int hum = humtemp[1];
      String testPayload = "{\
        \"test\": 1, \
        \"message\": \"Test ok, nhiệt độ: "+String(temp)+", độ ẩm: "+String(hum)+", wifi: "+WiFi.SSID()+", strength: "+String(dBmtoPercentage(WiFi.RSSI()))+"%\"\
      }";
      mqttClient.publish(EVENT_PUMP, (char*) testPayload.c_str());
    }

    if(hasReset){
      if(mqttDoc["reset"]){
        ESP.restart();
      }
    }

    if(hasUserA){
      String userAMessage = "UserA đang tưới";
      if(mqttDoc["userA"]){

        hasUserAPumpTime = !mqttDoc["userAPumpTime"].isNull();
        if(hasUserAPumpTime){
          userAPumpTime = mqttDoc["userAPumpTime"];
        }
        else{
          userAPumpTime = 0;
        }

        digitalWrite(USERAPUMPPIN, 1);
        userAStatus = String(digitalRead(USERAPUMPPIN));
        userAMessage = "UserA đang tưới";
      }
      else{
        digitalWrite(USERAPUMPPIN, 0);
        hasUserAPumpTime = false;
        userAPumpTime = 0;
        userAStatus = String(digitalRead(USERAPUMPPIN));
        userAMessage = "UserA dừng tưới";
      }
      // new SendDiscord(userAMessage);
      String pumpStatusPayload = "{\"userA\":"+userAStatus+"}";
      mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
    }

    if(hasUserB){
      String userBMessage = "UserC đang tưới";
      if(mqttDoc["userB"]){

        hasUserBPumpTime = !mqttDoc["userBPumpTime"].isNull();
        if(hasUserBPumpTime){
          userBPumpTime = mqttDoc["userBPumpTime"];
        }
        else{
          userBPumpTime = 0;
        }
        
        digitalWrite(USERBPUMPPIN, 1);
        userBStatus = String(digitalRead(USERBPUMPPIN));
        userBMessage = "UserC đang tưới";
      }
      else{
        digitalWrite(USERBPUMPPIN, 0);
        hasUserBPumpTime = false;
        userBPumpTime = 0;
        userBStatus = String(digitalRead(USERBPUMPPIN));
        userBMessage = "UserC dừng tưới";
      }
      // new SendDiscord(userBMessage);
      String pumpStatusPayload = "{\"userB\":"+userBStatus+"}";
      mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
    }

    if(hasUserC){
      String userCMessage = "UserC đang tưới";
      if(mqttDoc["userC"]){

        userCPumpTime = !mqttDoc["userCPumpTime"].isNull();
        if(userCPumpTime){
          userCPumpTime = mqttDoc["userCPumpTime"];
        }
        else{
          userCPumpTime = 0;
        }

        digitalWrite(USERCPUMPPIN, 1);
        userCStatus = String(digitalRead(USERCPUMPPIN));
        userCMessage = "UserC dừng tưới";
      }
      else{
        digitalWrite(USERCPUMPPIN, 0);
        userCPumpTime = false;
        userCPumpTime = 0;
        userCStatus = String(digitalRead(USERCPUMPPIN));
        userCMessage = "UserC dừng tưới";
      }
      // new SendDiscord(userCMessage);
      String pumpStatusPayload = "{\"userC\":"+userCStatus+"}";
      mqttClient.publish(EVENT_PUMP, (char*) pumpStatusPayload.c_str());
    }
  }
}