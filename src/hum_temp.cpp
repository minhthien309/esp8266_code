#include "hum_temp.h"

Adafruit_SHT31 sht30 = Adafruit_SHT31();

HumTemp::HumTemp(uint8_t address){
  Wire.begin();
	_address=address;
}

// float * HumTemp::getHumTemp(){
//   static float result[2];
//   if(!sht30.begin(_address)){
//     result[0] = -2;
//     result[1] = -2;
//     return result;
//   }

//   float t = sht30.readTemperature();
//   float h = sht30.readHumidity();

//   if(!isnan(t)){
//     temperature = t;
//   }
//   if(!isnan(h)){
//     humidity = h;
//   }

//   result[0] = temperature;
//   result[1] = humidity;

//   return result;
// }

int * HumTemp::getHumTemp(){
  unsigned int data[6];
  static int result[2];

	// Start I2C Transmission
	Wire.beginTransmission(_address);
	// Send measurement command
	Wire.write(0x2C);
	Wire.write(0x06);
	// Stop I2C transmission
	if (Wire.endTransmission()!=0) {
    result[0] = -1;
    result[1] = -1;
    return result;
  }
		

	// delay(500);

	// Request 6 bytes of data
	Wire.requestFrom(_address, 6);

	// Read 6 bytes of data
	// cTemp msb, cTemp lsb, cTemp crc, humidity msb, humidity lsb, humidity crc
	for (int i=0;i<6;i++) {
		data[i]=Wire.read();
	};
	
	delay(50);
	
	if (Wire.available()!=0) {
    result[0] = -2;
    result[1] = -2;
    return result;
  }

	// Convert the data
	result[0] = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45; //1: temperature
	result[1] = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0); //0: humidity

	return result;
}