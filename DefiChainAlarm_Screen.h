#ifndef DEFICHAINALARM_SCREEN
#define DEFICHAINALARM_SCREEN

#include "Arduino.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define NUMOF_SYSTEM_MSG  4
#define NUMOF_SYSTEM_MSG_CHARS 59


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
    WiFiUDP _ntpUDP;
    NTPClient _timeClient;
    int _OldVaultRatio;
    int _OldNextVaultRatio;
    int _OldVaultLimit;
    float _OldDusdFee;
    float _OldDfiPrice;
};

#endif
