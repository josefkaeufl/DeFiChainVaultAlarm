#ifndef DEFICHAINALARM_TELEGRAM
#define DEFICHAINALARM_TELEGRAM

#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include "DefiChainAlarm_Cfg.h"
#include "DefiChainAlarm_Vault.h"
#include "DefiChainAlarm_Screen.h"

class DefiChainAlarm_Telegram
{
  public:
    DefiChainAlarm_Telegram();
    void Init();
    void Cyclic(DefiChainAlarm_Screen& Screen, DefiChainAlarm_Vault& Vault);
    void SendMsg(const char* text);
    void SendMsgWithReplyKeyboard(const char* text);
    bool GetProtocolStatus();
    bool _ShallRefreshRatio;
    bool _ShallRefreshRatioNow;
    
  private:
    void _handleNewMessages(int numNewMessages, DefiChainAlarm_Screen& Screen, DefiChainAlarm_Vault& Vault);
    bool _connectWifi();
    int _botRequestDelay;
    unsigned long _lastTimeBotRan;
    bool _ReadVaultLimit;
    bool _ReadWlanSsid;
    bool _ReadWlanPswd;
    bool _ReadVaultId;
    bool _ReadAddr;
    bool _SendRefreshedRatio;
    UniversalTelegramBot _TelegramBot;
    WiFiClientSecure _WifiClient;
};

#endif
