#ifndef hum_temp_h
#define hum_temp_h
#include "define_const.h"

#include "Wire.h"
#include "Arduino.h"
#include "Adafruit_SHT31.h"

class HumTemp{
  protected:
    void setHumidity(float hum);
    void setTemperature(float temp);
  private:
    uint8_t _address;
    float temperature;
    float humidity;
  public:
    HumTemp(uint8_t address);
    // float * getHumTemp();
    int * getHumTemp();
};

#endif