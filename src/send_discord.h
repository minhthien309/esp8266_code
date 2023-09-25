#ifndef send_discord_h
#define send_discord_h

#include "define_const.h"
#include <AsyncHttpClient.h>

class SendDiscord {
  public:
   SendDiscord(String message);
   void SendHumTemp(int hum, int temp);
};

#endif