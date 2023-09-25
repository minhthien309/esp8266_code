#include "device_wifi.h"

struct wifis {
  wifis(char* u, char* p){
    ssid = u;
    pass = p;
  };
  char* ssid;
  char* pass;
};

wifis wifi[] = {
  wifis((char*)"***********", (char*)"**************")
};

int timeRetryConnect = 0;

DeviceWifi::DeviceWifi(){

}

void DeviceWifi::startWiFi(){
  for(unsigned int i = 0; i < (sizeof (wifi) / sizeof(wifis)); i++){
    // Serial.println((sizeof (wifi) / sizeof(wifis)));
    // Serial.println((sizeof wifi / sizeof wifi[0]));
    // char *ssid = wifi[i][0];
    // char *password = wifi[i][1];
    char *ssid = wifi[i].ssid;
    char *password = wifi[i].pass;

    Serial.println("------------------------------");
    Serial.print("ssid: ");
    Serial.println(ssid);
    Serial.print("password: ");
    Serial.println(password);
    Serial.println("------------------------------");

    WiFi.begin(ssid, password);           // Kết nối vào mạng WiFi
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.print("\n");
    // Chờ kết nối WiFi được thiết lập

    while (WiFi.status() != WL_CONNECTED) {
      if(timeRetryConnect > 30){
        timeRetryConnect = 0;
        break;
      }
      delay(1000);
      Serial.print(".");
      timeRetryConnect++;
    }
  }
  if(WiFi.status() != WL_CONNECTED){
    ESP.restart();
  }
  Serial.println("\n");
  Serial.println("Connection established!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());       // Gởi địa chỉ IP đến máy tinh
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}