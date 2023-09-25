#include "helper.h"

String secondsToTime(int totalSeconds){
  int seconds = totalSeconds % 60; 
  int minutes = (totalSeconds / 60) % 60; 
  int hours = (totalSeconds % 86400) / 3600; 
  int days = (totalSeconds % (86400 * 30)) / 86400;
  
  char result[50];
  
  sprintf(result, "%02d ngày %02d giờ %02d phút %02d giây", days, hours, minutes, seconds);
  
  return String(result);
}

int dBmtoPercentage(int dBm)
{
  int quality;
  if(dBm <= RSSI_MIN)
  {
      quality = 0;
  }
  else if(dBm >= RSSI_MAX)
  {  
      quality = 100;
  }
  else
  {
      quality = 2 * (dBm + 100);
  }

    return quality;
}