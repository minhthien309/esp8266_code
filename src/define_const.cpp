#include "define_const.h"

DefineConst::DefineConst(){
  this->userAMaxPumpTime = 250;
  this->userACurrentPumpTime = 0;
  this->temp = 0.0;
  this->hum = 0.0;
}

int DefineConst::getUserAMaxPumpTime(){
  return this->userAMaxPumpTime;
}

void DefineConst::setUserAMaxPumpTime(int time){
  this->userAMaxPumpTime = time;
}

int DefineConst::getUserACurrentPumpTime(){
  return this->userACurrentPumpTime;
}

void DefineConst::setUserACurrentPumpTime(int time){
  this->userACurrentPumpTime = time;
}

float DefineConst::getHum(){
  return this->hum;
}

void DefineConst::setHum(float hum){
  this->hum = hum;
}

float DefineConst::getTemp(){
  return this->temp;
}

void DefineConst::setTemp(float temp){
  this->temp = temp;
}

