#include "Arduino.h"
#include "DefiChainAlarm_Telegram.h"

#include "DefiChainAlarm_Cfg.h"
#include "DefiChainAlarm_Vault.h"
#include "DefiChainAlarm_Screen.h"
#include "DefiChainAlarm_Eeprom.h"
#include "DefiChainAlarm_Http.h"

#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

WiFiClientSecure p_WifiClient;
UniversalTelegramBot p_TelegramBot(BOTtoken, p_WifiClient);

DefiChainAlarm_Telegram::DefiChainAlarm_Telegram()
  : _TelegramBot(BOTtoken, _WifiClient)
{
  // Checks for new messages every 1 second.
  _botRequestDelay = 1000;

  // init control flags
  _ReadVaultLimit = false;
  _ReadWlanSsid = false;
  _ReadWlanPswd = false;
  _ReadVaultId = false;
  _ReadAddr = false;
  _ShallRefreshRatio = false;
  _ShallRefreshRatioNow = false;
  _SendRefreshedRatio = false;
}

void DefiChainAlarm_Telegram::Init()
{
  int WifiConnectStartTime;
  
  // load values from eeprom
  DefiChainAlarm_Eeprom Eeprom;

  WifiConnectStartTime = millis();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(Eeprom.GetWlanSsid(), Eeprom.GetWlanPswd());
  
  _WifiClient.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");

    if( millis() - WifiConnectStartTime > 10000 )
    {
      // abort time 10sec
      Serial.println("Timeout. Use default values instead.");
      Serial.print("Connecting to WiFi");
      
      WiFi.begin(DEFAULT_WLAN_SSID, DEFAULT_WLAN_PASSWORD);
      
      WifiConnectStartTime = millis();
    }
  }
  //WiFi.setSleep(true); //power saving WIFI_PS_MIN_MODEM
  
  Serial.println(".ok");
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());

}

void DefiChainAlarm_Telegram::Cyclic(DefiChainAlarm_Screen& Screen, DefiChainAlarm_Vault& Vault)
{
  // Telegram
  if (millis() - _lastTimeBotRan > _botRequestDelay)
  {
    int numNewMessages = _TelegramBot.getUpdates(_TelegramBot.last_message_received + 1);

    while (numNewMessages)
    {
      _handleNewMessages(numNewMessages, Screen, Vault);
      numNewMessages = _TelegramBot.getUpdates(_TelegramBot.last_message_received + 1);
    }
    
    _lastTimeBotRan = millis();
  }
}

void DefiChainAlarm_Telegram::SendMsg(const char* text)
{
  _TelegramBot.sendMessage(CHAT_ID, text, "");
}

void DefiChainAlarm_Telegram::SendMsgWithReplyKeyboard(const char* text)
{
  String keyboardJson = "[[\"/start\"],[\"/refresh\", \"/vault\"],[\"/buzzertest\", \"/protocol\"],[\"/readlimit\", \"/setlimit\"],[\"/setwlanssid\", \"/setwlanpswd\"]]";
  _TelegramBot.sendMessageWithReplyKeyboard(CHAT_ID, text, "", keyboardJson, true);
}


//Handle what happens when you receive new messages
void DefiChainAlarm_Telegram::_handleNewMessages(int numNewMessages, DefiChainAlarm_Screen& Screen, DefiChainAlarm_Vault& Vault)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {

    // Chat id of the requester
    String chat_id = String(_TelegramBot.messages[i].chat_id);
    if ( chat_id != CHAT_ID )
    {
      SendMsg("I don't know you.");
      SendMsg("I shall not talk to strangers.");
      return;
    }
    else
    {
      // Print the received message
      String text = _TelegramBot.messages[i].text;
      Serial.println(text);

      String from_name = _TelegramBot.messages[i].from_name;

      if( _ReadVaultLimit == true )
      {
        int newVaultLimit;
        newVaultLimit = text.toInt();
        
        if(newVaultLimit!=0)
        {
          //set new limit
          DefiChainAlarm_Eeprom Eeprom;
          Eeprom.SetAlarmLimit(newVaultLimit);
          Vault.SetLimit(newVaultLimit);
          
          _ReadVaultLimit = false;
          SendMsg("New vault alarm limit stored.");
          Screen.AddSystemMessage("new vault alarm limit stored");  
          
          DefiChainAlarm_Http Http;
          int VaultRatio;
          int nextVaultRatio;
          float DfiPrice;
          float DusdFee;
          Http.GetVaultStatus(&VaultRatio, &nextVaultRatio);
          Http.GetDfiPrice(&DfiPrice);  
          Http.GetDusdFee(&DusdFee);  

          Screen.UpdateScreen(VaultRatio, nextVaultRatio, newVaultLimit, DusdFee, DfiPrice); 

          char * ErrorMessage;
          ErrorMessage = Vault.TestVaultStatus(nextVaultRatio);
          if(ErrorMessage[0] != '\0')
          {
            SendMsg(ErrorMessage);
          }
        }
        else
        {
          _ReadVaultLimit = false;
          SendMsg("Error: Not a number.");
        }
      }
      else if( _ReadWlanSsid == true )
      {
        // store value to eeprom
        DefiChainAlarm_Eeprom Eeprom;
        Eeprom.SetWlanSsid(text.c_str());
        
        _ReadWlanSsid = false;
        SendMsg("New WLAN SSID stored.");
        Screen.AddSystemMessage("new WLAN SSID stored"); 
        Screen.UpdateScreenMessages();
      }
      else if( _ReadWlanPswd == true )
      {
        // store value to eeprom
        DefiChainAlarm_Eeprom Eeprom;
        Eeprom.SetWlanPswd(text.c_str());
        
        _ReadWlanPswd = false;
        SendMsg("New WLAN password stored.");
        Screen.AddSystemMessage("new WLAN password stored"); 
        Screen.UpdateScreenMessages();
      }
      else if( _ReadVaultId == true )
      {
        // store id to eeprom
        DefiChainAlarm_Eeprom Eeprom;
        Eeprom.SetDefichainVault(text.c_str());

        _ReadVaultId = false;
        SendMsg("New vault id stored.");
        Screen.AddSystemMessage("new vault id stored"); 
        Screen.UpdateScreenMessages();
      }
      else if( _ReadAddr == true )
      {
        // store addr to eeprom
        DefiChainAlarm_Eeprom Eeprom;
        Eeprom.SetDefichainAddr(text.c_str());

        _ReadAddr = false;
        SendMsg("New address stored.");
        Screen.AddSystemMessage("new address stored"); 
        Screen.UpdateScreenMessages();
      }
      else if (text == "/start")
      {
        String welcome = "Welcome, " + from_name + ".\n";
        welcome += "Use the following commands.\n\n";
        welcome += "/start to print this message again \n";
        welcome += "/vault to print the vault defiscan.live url to your vault \n";
        welcome += "/refresh to get the current vault status \n";
        welcome += "/buzzertest to test if the buzzer is working \n";
        welcome += "/protocol will send you the updated vault status \n";
        welcome += "/readlimit show the current vault alarm limit \n";
        welcome += "/setlimit will allow you to change the vault alarm limit \n";
        welcome += "/setwlanssid will allow you to change the WLAN SSID to connect to \n";
        welcome += "/setwlanpswd will allow you to change the WLAN password \n";
        welcome += "/setdefichainvaultid will allow you to change the vault id \n";
        welcome += "/setdefichainaddr will allow you to change the address realted to the vault \n";
        welcome += "/sendkeyboard will send this commands as special keyboard \n";
        SendMsg(welcome.c_str());
      }
      else if (text == "/buzzertest")
      {
        SendMsg("Buzzer activated...");
        pinMode(BUZZERPIN, OUTPUT);  
        analogWrite(BUZZERPIN, 10);
        delay(1000);
        analogWrite(BUZZERPIN, 0);
        pinMode(BUZZERPIN, INPUT);  
        SendMsg("Buzzer deactivated...");
        Screen.AddSystemMessage("buzzer tested"); 
        Screen.UpdateScreenMessages(); 
      }
      else if (text == "/vault")
      {
        SendMsg(Vault.GetVaultInfoLink());
      }
      else if (text == "/refresh")
      {
        _ShallRefreshRatio = true;
        _ShallRefreshRatioNow = true;

        SendMsg("Just a second. I will check your vault status. (This may take some time in case site response is invalid)");
        Screen.AddSystemMessage("vault status refresh requested"); 
        Screen.UpdateScreenMessages(); 
      }
      else if (text == "/protocol")
      {
        if ( _SendRefreshedRatio == false )
        {
          _SendRefreshedRatio = true;
          SendMsg("Protocol mode ON");
          Screen.AddSystemMessage("protocol mode on"); 
          Screen.UpdateScreenMessages(); 
        }
        else
        {
          _SendRefreshedRatio = false;
          SendMsg("Protocol mode OFF");
          Screen.AddSystemMessage("protocol mode off"); 
          Screen.UpdateScreenMessages(); 
        }
      }
      else if (text == "/readlimit")
      {
        int VaultLimit = 0;
        DefiChainAlarm_Eeprom Eeprom;
        VaultLimit = Eeprom.GetAlarmLimit();
        SendMsg(("The currently configured vault alarm limit is " + String(VaultLimit) + "%").c_str());
      }
      else if (text == "/setlimit")
      {
        SendMsg("Please write new vault alarm limit.");
        _ReadVaultLimit = true;
      }
      else if (text == "/setwlanssid")
      {
        SendMsg("Please write new vault WLAN SSID. (will be used after next reset)");
        _ReadWlanSsid = true;
      }
      else if (text == "/setwlanpswd")
      {
        SendMsg("Please write new vault WLAN password. (will be used after next reset)");
        _ReadWlanPswd = true;
      }
      else if (text == "/setdefichainvaultid")
      {
        SendMsg("Please write new DeFiChain vault Id. (shall match to DeFiChain address)");
        _ReadVaultId = true;
      }
      else if (text == "/setdefichainaddr")
      {
        SendMsg("Please write new DeFiChain address. (shall match to DeFiChain vault Id)");
        _ReadAddr = true;
      }
      else if (text == "/sendkeyboard")
      {
        SendMsgWithReplyKeyboard("Done. Alternative keyboard should be avaliable now.");
      }
    }

  }
}
