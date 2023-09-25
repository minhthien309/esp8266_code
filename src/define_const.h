#ifndef define_const_h
#define define_const_h

#define RAINSENSOR A0
#define USERCPUMPPIN 13 //D7
#define USERBPUMPPIN 12 //D6
#define USERAPUMPPIN 14 //D5
#define DHTPIN 13
#define I2CADDRESS 0x44

#define MQTT_BROKER "mqtt.**********.com"
#define MQTT_USERNAME "***********"
#define MQTT_PASSWORD "***********"
#define MQTT_PORT 309
#define EVENT_PUMP "esp/pump"
#define EVENT_PUMP_CONTROL "esp/pump_control"
#define EVENT_WEATHER "esp/weather"
#define EVENT_MESSAGE "esp/message"
#define EVENT_RAIN "esp/rain"

const int RSSI_MAX =-50;// define maximum strength of signal in dBm
const int RSSI_MIN =-100;// define minimum strength of signal in dBm

extern int userAMaxPumpTime;
extern int userACurrentPumpTime;

extern float temp;
extern float hum;

#define HOMEURL "http://****.*****.com/webhook/auto-watering"
#define DISCORDURL HOMEURL"/send-discord"
#define HUMTEMPURL HOMEURL"/store-dht"

class DefineConst{
  private:
    int userAMaxPumpTime;
    int userACurrentPumpTime;
    float temp = 0.0;
    float hum = 0.0;
  public:
    DefineConst();
    int getUserAMaxPumpTime();
    void setUserAMaxPumpTime(int time);
    int getUserACurrentPumpTime();
    void setUserACurrentPumpTime(int time);
    float getHum();
    void setHum(float hum);
    float getTemp();
    void setTemp(float temp);
};

#endif
