#include "web_server.h"

AsyncWebServer server(8080);
AsyncHttpClient aClient;
DefineConst defineConst;
WiFiUDP UDP;
WakeOnLan WOL(UDP);
// MQTT mqtt1;

const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
    <head>
      <title>Smart Board</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <meta charset="UTF-8">
      <link rel="icon" href="data:,">
      <style>
        html {font-family: Arial; display: inline-block; text-align: center;}
        h2 {font-size: 3.0rem;}
        p {font-size: 3.0rem;}
        body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
        .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
        .switch input {display: none}
        .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
        .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
        input:checked+.slider {background-color: #b30000}
        input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
      </style>
    </head>
    <body>
      <h2>Tự động tưới</h2>
    </body>
  </html>
)rawliteral";
  
WebServer::WebServer(){
  // init();
}

void WebServer::handler(){
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Get DHT data
  server.on("/get-dht-data", HTTP_GET, [] (AsyncWebServerRequest *request) {
    float hum = defineConst.getHum();
    float temp = defineConst.getTemp();
    String postData = "{\"hum\":\"" + String(hum) + "\", \"temp\":\"" + String(temp) + "\"}";
    request->send(200, "text/plain", postData);   
  });

  server.on("/reset", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String pumpMessage = "Remote reset";
    ESP.restart();

    String postData = "{\"content\":\"" + pumpMessage + "\"}";
    aClient.init("POST", DISCORDURL, "application/json", postData);
    aClient.send();
    
    request->send(200, "text/plain", "OK");
  });

  server.on("/wakeup", HTTP_GET, [] (AsyncWebServerRequest *request) {
    const char *MACAddress = "90:1B:0E:C4:2D:B1";

    WOL.sendMagicPacket(MACAddress); 
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

// const char* WebServer::render(){

// }