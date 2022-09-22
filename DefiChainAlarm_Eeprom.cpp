#include "Arduino.h"
#include "DefiChainAlarm_Eeprom.h"
#include "DefiChainAlarm_Cfg.h"


#define LOCATION_ALARMLIMIT  "VAULT_LIMIT"
#define LOCATION_WLANSSID    "WLAN_SSID"
#define LOCATION_WLANPSWD    "WLAN_PASSWORD"
#define LOCATION_DFIVAULT    "DFI_VAULT"
#define LOCATION_DFIADDR     "DFI_ADDR"


void DefiChainAlarm_Eeprom::SetAlarmLimit(int i_Limit)
{
  _preferences.begin("DefiAlarm", false); 
  _preferences.putInt(LOCATION_ALARMLIMIT, i_Limit);
  _preferences.end();
}

int DefiChainAlarm_Eeprom::GetAlarmLimit(void)
{
  int retval;
  
  _preferences.begin("DefiAlarm", false); 

  retval = _preferences.getInt(LOCATION_ALARMLIMIT, 0);
  if( retval == 0 )
  {
    Serial.println("EEPROM was empty. Using default limit of 200%.");
    retval = 200;
    _preferences.putInt(LOCATION_ALARMLIMIT, retval);
  }
  Serial.println(retval);
  _preferences.end();
  
  return retval;
}



void DefiChainAlarm_Eeprom::SetWlanSsid(const char* pc_Ssid)
{
  _preferences.begin("DefiAlarm", false); 
  _preferences.putString(LOCATION_WLANSSID, pc_Ssid);
  _preferences.end();
}

char* DefiChainAlarm_Eeprom::GetWlanSsid(void)
{
  static char retval[256];
  
  _preferences.begin("DefiAlarm", false); 

  strcpy(retval, _preferences.getString(LOCATION_WLANSSID, "").c_str());
  if( retval[0] == '\0' )
  {
    Serial.println("Wlan SSID was empty. Using default SSID instead!");
    strcpy(retval, DEFAULT_WLAN_SSID);
  }
  _preferences.end();
  
  return retval;
}




void DefiChainAlarm_Eeprom::SetWlanPswd(const char* pc_Pswd)
{ 
  _preferences.begin("DefiAlarm", false); 
  _preferences.putString(LOCATION_WLANPSWD, pc_Pswd);
  _preferences.end();
}

char* DefiChainAlarm_Eeprom::GetWlanPswd(void)
{
  static char retval[256];

  retval[0] = 0xFF;
  
  _preferences.begin("DefiAlarm", false); 

  strcpy(retval, _preferences.getString(LOCATION_WLANPSWD, "").c_str());
  if( retval[0] == '\0' )
  {
    Serial.println("Wlan password was empty. Using default password instead!");
    strcpy(retval, DEFAULT_WLAN_PASSWORD);
  }
  _preferences.end();
  
  return retval;
}


void DefiChainAlarm_Eeprom::SetDefichainVault(const char* pc_Vault)
{
  _preferences.begin("DefiAlarm", false); 
  _preferences.putString(LOCATION_DFIVAULT, pc_Vault);
  _preferences.end();
}

char* DefiChainAlarm_Eeprom::GetDefichainVault(void)
{
  static char retval[256];

  retval[0] = 0xFF;
  
  _preferences.begin("DefiAlarm", false); 

  strcpy(retval, _preferences.getString(LOCATION_DFIVAULT, "").c_str());
  if( retval[0] == '\0' )
  {
    Serial.println("Defichain Vault ID was empty. Using default ID instead!");
    strcpy(retval, DEFAULT_DEFICHAIN_VAULT);
  }
  _preferences.end();
  
  return retval;
}


void DefiChainAlarm_Eeprom::SetDefichainAddr(const char* pc_Addr)
{
  _preferences.begin("DefiAlarm", false); 
  _preferences.putString(LOCATION_DFIADDR, pc_Addr);
  _preferences.end();
}

char* DefiChainAlarm_Eeprom::GetDefichainAddr(void)
{
  static char retval[256];

  retval[0] = 0xFF;
  
  _preferences.begin("DefiAlarm", false); 

  strcpy(retval, _preferences.getString(LOCATION_DFIADDR, "").c_str());
  if( retval[0] == '\0' )
  {
    Serial.println("Defichain address was empty. Using default address instead!");
    strcpy(retval, DEFAULT_DEFICHAIN_ADDR);
  }
  _preferences.end();
  
  return retval;
}
