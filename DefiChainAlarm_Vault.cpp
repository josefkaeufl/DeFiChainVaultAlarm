#include "Arduino.h"
#include "DefiChainAlarm_Vault.h"
#include "DefiChainAlarm_Eeprom.h"
#include "DefiChainAlarm_Cfg.h"

DefiChainAlarm_Vault::DefiChainAlarm_Vault()
{
  DefiChainAlarm_Eeprom Eeprom;
  
  _TestRetryCounter = 0;
  _VaultLimit = Eeprom.GetAlarmLimit();
}

void DefiChainAlarm_Vault::SetLimit(int Limit)
{
  _VaultLimit = Limit;
}

int DefiChainAlarm_Vault::GetLimit(void)
{
  return _VaultLimit;
}

char* DefiChainAlarm_Vault::GetVaultInfoLink(void)
{
  DefiChainAlarm_Eeprom Eeprom;
  
  static char vaultUrl[256];
  strcpy(vaultUrl, "Your vault: ");
  strcat(vaultUrl, "https://defiscan.live/vaults/");
  strcat(vaultUrl, Eeprom.GetDefichainVault() );
  return vaultUrl;
}

char* DefiChainAlarm_Vault::TestVaultStatus(int nextVaultRatio)
{
  static char returnMessage[256];
  
  returnMessage[0] = '\0';
  
  if (nextVaultRatio != -1)
  { 
    if (nextVaultRatio < _VaultLimit)
    {
      //ALARM
      analogWrite(BUZZERPIN, 20);
      Serial.print("Next Ratio: ");
      Serial.println(nextVaultRatio);
      Serial.print("Vault Limit: ");
      Serial.println(_VaultLimit);
      strcpy(returnMessage, ("ALARM! Your vault is at " + String(nextVaultRatio) + "%").c_str());
    }

    _TestRetryCounter = 0;
  }
  else if(nextVaultRatio == -1)
  { 
    //SOFT-ALARM
    strcpy(returnMessage, "WARNING! Vault read error. Will retry in 5min.");

    if(_TestRetryCounter > 6)
    {
      //ALARM
      strcpy(returnMessage, "ALARM! Vault read error. Check internet connection.");
      analogWrite(BUZZERPIN, 20);
    }
    else
    {
      _TestRetryCounter++;
    }
    
  }
  else
  {
    //its fine
    Serial.print("Vault Limit: ");
    Serial.println(_VaultLimit);
  }

  return returnMessage; 
}
