#include "web_socket.h"

WebSocketsServer webSocket = WebSocketsServer(3009);
DynamicJsonDocument doc(1024);

WebSocket::WebSocket(){
}

void WebSocket::init(){
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void WebSocket::webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  switch (type) {
    case WStype_DISCONNECTED:                         // Sự kiện khi client ngắt kết nối
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:                            // Sự kiện khi client kết nối
      {
        // Gởi địa chỉ IP đến máy tinh
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        webSocket.sendTXT(num, "Connected");          // Thông báo kết nối thành công cho client

        DynamicJsonDocument pumpStatus(64);
        pumpStatus["pumpStatus"]["userA"] = digitalRead(USERAPUMPPIN);
        pumpStatus["pumpStatus"]["userB"] = digitalRead(USERBPUMPPIN);
        pumpStatus["pumpStatus"]["userC"] = digitalRead(USERCPUMPPIN);
        String response = "";
        serializeJson(pumpStatus, response);
        webSocket.sendTXT(num, response);
      }
      break;
    case WStype_TEXT:                                 // Sự kiện khi client gởi tin nhắn dạng TEXT
      //      Serial.printf("[%u] get text: %s\n", num, payload);
      receiveCommand(num, payload);
      break;
    case WStype_BIN:                                  // Sự kiện khi client gởi tin nhắn dạng BINARY
      Serial.printf("[%u] get binary length: %u\n", num, length);

      hexdump(payload, length);
      // webSocket.sendBIN(num, payload, length);
      break;
  }
}

void WebSocket::receiveCommand(uint8_t num, uint8_t * payload) {
  String message = (char*)payload;
  if (message == "__ping__") {
    webSocket.sendTXT(num, "__pong__");
  }
  else {
    DeserializationError error = deserializeJson(doc, payload);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String pumpMessage = "";

    if (doc["command"]) {
      bool hasUserA = !doc["command"]["userA"].isNull();
      bool hasUserB = !doc["command"]["userB"].isNull();
      bool hasUserC = !doc["command"]["userC"].isNull();
      if (hasUserA) {
        digitalWrite(USERAPUMPPIN, !doc["command"]["userA"]);
        if (digitalRead(USERAPUMPPIN)) {
          pumpMessage = "UserA đang tưới";
        }
        else {
          pumpMessage = "UserA dừng tưới";
        }
      }
      if (hasUserC) {
        digitalWrite(USERCPUMPPIN, !doc["command"]["userC"]);
        if (digitalRead(USERCPUMPPIN)) {
          pumpMessage = "UserC đang tưới";
        }
        else {
          pumpMessage = "UserC dừng tưới";
        }
      }
      if (hasUserB) {
        digitalWrite(USERBPUMPPIN, !doc["command"]["userB"]);
        if (digitalRead(USERBPUMPPIN)) {
          pumpMessage = "UserC đang tưới";
        }
        else {
          pumpMessage = "UserC dừng tưới";
        }
      }

      //Send state to web socket
      DynamicJsonDocument pumpStatus(64);
      pumpStatus["pumpStatus"]["userA"] = digitalRead(USERAPUMPPIN);
      pumpStatus["pumpStatus"]["userB"] = digitalRead(USERBPUMPPIN);
      pumpStatus["pumpStatus"]["userC"] = digitalRead(USERCPUMPPIN);
      String response = "";
      serializeJson(pumpStatus, response);
      webSocket.broadcastTXT(response);

      //Send to Discord bot
      new SendDiscord(pumpMessage);
    }
  }
}