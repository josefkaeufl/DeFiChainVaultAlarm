
#include "DefiChainAlarm_Cfg.h"
#include "DefiChainAlarm_Eeprom.h"
#include "DefiChainAlarm_Screen.h"
#include "DefiChainAlarm_Telegram.h"
#include "DefiChainAlarm_Http.h"
#include "DefiChainAlarm_Vault.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>


/* ****************** public variables ******************** */

bool p_ShallRefreshRatio = false;
bool p_ShallRefreshRatioNow = false;

int p_VaultRefreshDelay = 1000 * 60 * 30;
unsigned long p_LastVaultRefreshRan = 0;

int p_VaultRefreshRetryDelay = 1000 * 60 * 2;
unsigned long p_LastVaultRefreshRetryRan;

int p_NextVaultRatio = 0;
int p_CurrentVaultRatio = 0;
float p_DusdFee = 0;
float p_DfiPrice = 0;

bool p_ReadVaultLimit = false;
bool p_ReadWlanSsid = false;
bool p_ReadWlanPswd = false;
bool p_FirstRefreshRun = false;


/* **************** main functions **************** */
void setup()
{  
  Serial.begin(115200);

  Serial.println("");
}


void loop()
{ 
  /* Note: This has been in the setup() functon but somehow the screen object creation 
   *  failed all the time. Here it works. Don't know why...
   */
  DefiChainAlarm_Vault Vault;
  DefiChainAlarm_Telegram Telegram;

  DefiChainAlarm_Eeprom Eeprom;
  DefiChainAlarm_Http Http;

  Serial.println("Initialize Telegram bot");
  Telegram.Init();
  
  Serial.println("");
  Serial.println("Update display");
  DefiChainAlarm_Screen Screen;
  Screen.AddSystemMessage("initialization done"); 
  Screen.AddSystemMessage("fetching vault data..."); 
  Screen.UpdateScreen(0,0,0,0,0);

  while(1)
  {
    // check for new telegram messages
    Telegram.Cyclic(Screen, Vault);
    
    // Vault status
    if(   ( millis() > p_LastVaultRefreshRan + p_VaultRefreshDelay ) 
        || ( p_FirstRefreshRun == false )
      )
    {
      char* VaultTestErrorMsg;
      
      p_FirstRefreshRun = true;
  
      Http.GetVaultStatus(&p_CurrentVaultRatio, &p_NextVaultRatio);   
      Http.GetDfiPrice(&p_DfiPrice);  
      Http.GetDusdFee(&p_DusdFee);  
       
      VaultTestErrorMsg = Vault.TestVaultStatus(p_NextVaultRatio);
  
      if( VaultTestErrorMsg[0] == '\0' )
      {
        Screen.AddSystemMessage("vault status update ok"); 
        p_LastVaultRefreshRan = millis();
      }
      else
      {
        Screen.AddSystemMessage(VaultTestErrorMsg);
        p_LastVaultRefreshRan = millis() - p_VaultRefreshDelay + p_VaultRefreshRetryDelay;
      }
  
      Screen.UpdateScreen(p_CurrentVaultRatio, p_NextVaultRatio, Eeprom.GetAlarmLimit(), p_DusdFee, p_DfiPrice); 
  
      if (Telegram.GetProtocolStatus() == true)
      {
        Telegram.SendMsg((String(p_NextVaultRatio) + "%").c_str());
      }
      
    }
  
  
    if ( Telegram._ShallRefreshRatio != false )
    {
      if ( (millis() > p_LastVaultRefreshRetryRan + p_VaultRefreshRetryDelay) || (Telegram._ShallRefreshRatioNow == true) )
      {
        char* VaultTestErrorMsg;
        
        Http.GetVaultStatus(&p_CurrentVaultRatio, &p_NextVaultRatio);
        Http.GetDfiPrice(&p_DfiPrice);  
        Http.GetDusdFee(&p_DusdFee);  
  
        Screen.UpdateScreen(p_CurrentVaultRatio, p_NextVaultRatio, Eeprom.GetAlarmLimit(), p_DusdFee, p_DfiPrice);
        VaultTestErrorMsg = Vault.TestVaultStatus(p_NextVaultRatio);
        
        if(Telegram._ShallRefreshRatioNow == true)
        {
          if( VaultTestErrorMsg[0] == '\0' )
          {
            Screen.AddSystemMessage("vault status update ok"); 
            Telegram.SendMsg(("Next vault status: " + String(p_NextVaultRatio) + "%").c_str());
            Telegram._ShallRefreshRatio = false;
            Telegram._ShallRefreshRatioNow = false;
          }
          else
          {
            Screen.AddSystemMessage(VaultTestErrorMsg);
            Telegram._ShallRefreshRatio = true;
            Telegram._ShallRefreshRatioNow = false;
            p_LastVaultRefreshRetryRan = millis();
          }
        }
        else
        {
            Telegram._ShallRefreshRatio = false;
            Telegram._ShallRefreshRatioNow = false; 
        }
        
      }
    }
  //
  //   // power saving
  ////   setCpuFrequencyMhz(40);
  ////   lastwaitstart = millis();
  ////   while( (lastwaitstart + (1000*2)) > millis() )
  ////   {
  ////      //wait
  ////   }
  ////   setCpuFrequencyMhz(240);
  }
}
