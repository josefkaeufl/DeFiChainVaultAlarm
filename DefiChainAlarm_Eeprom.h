#ifndef DEFICHAINALARM_EEPROM
#define DEFICHAINALARM_EEPROM

#include "Arduino.h"
#include <Preferences.h>

class DefiChainAlarm_Eeprom
{
  public:
  
    void SetAlarmLimit(int i_Limit);
    int GetAlarmLimit(void); 

    void SetWlanSsid(const char* pc_Ssid);
    char* GetWlanSsid(void); 

    void SetWlanPswd(const char* pc_Pswd);
    char* GetWlanPswd(void); 

    void SetDefichainVault(const char* pc_Vault);
    char* GetDefichainVault(void); 

    void SetDefichainAddr(const char* pc_Addr);
    char* GetDefichainAddr(void); 
    
  private:
    Preferences _preferences;
};

#endif
