#ifndef DEFICHAINALARM_SCREEN
#define DEFICHAINALARM_SCREEN

#include "Arduino.h"
//#include <NTPClient.h>
//#include <WiFiUdp.h>

/* Configuration of screen system messages */
#define NUMOF_SYSTEM_MSG  4
#define NUMOF_SYSTEM_MSG_CHARS 59

/* Configuration of NTP */
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"  


class DefiChainAlarm_Screen
{
  public:
    DefiChainAlarm_Screen();
    void UpdateScreen(int VaultRatio, int nextVaultRatio, int VaultLimit, float DusdFee, float DfiPrice);
    void UpdateScreenMessages(void);
    void AddSystemMessage(const char* Message);
    
  private:
    char _SystemMessages[NUMOF_SYSTEM_MSG][NUMOF_SYSTEM_MSG_CHARS];
    uint8_t* _framebuffer;
    int _OldVaultRatio;
    int _OldNextVaultRatio;
    int _OldVaultLimit;
    float _OldDusdFee;
    float _OldDfiPrice;
};

#endif
