#include "Arduino.h"
#include "DefiChainAlarm_Http.h"
#include "DefiChainAlarm_Eeprom.h"

#include "DefiChainAlarm_Cfg.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>


void DefiChainAlarm_Http::GetVaultStatus(int* VaultRatio, int* nextVaultRatio)
{
  bool isVaultFound = false;
  bool isNextRatioDone = false;
  bool isCurrentRatioDone = false;

  DefiChainAlarm_Eeprom Eeprom;
  const char *p_host = "ocean.defichain.com";
  const String p_getPrefix = "/v0/mainnet/address/";
  const String p_getSuffix = "/vaults";
  const int p_httpsPort = 443;
  
  String link = p_getPrefix + Eeprom.GetDefichainAddr() + p_getSuffix;

  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(p_host);

  httpsClient.setInsecure();

  Serial.println("HTTPS Connecting");
  int r = 0; //retry counter
  while ((!httpsClient.connect(p_host, p_httpsPort)) && (r < 30))
  {
    delay(100);
    Serial.print(".");
    r++;
  }
  if (r == 30)
  {
    Serial.println("Connection failed");
  }
  else
  {
    Serial.println("Connected to web");

    Serial.print("read vault status");
    Serial.print("requesting URL: ");
    Serial.println(p_host);

    httpsClient.setTimeout(5000);

    httpsClient.print(String("GET ") + link + " HTTP/1.1\r\n" +
                      "Host: " + p_host + "\r\n" +
                      "User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_1 like Mac OS X) AppleWebKit/603.1.30 (KHTML, like Gecko) Version/10.0 Mobile/14E304 Safari/602.1\r\n" +
                      "Connection: close\r\n\r\n");

    //Serial.println("request sent");

    while (httpsClient.connected())
    {
      String line = httpsClient.readStringUntil('\n');
      //Serial.println(line); //Print response
      if (line == "\r")
      {
        //Serial.println("headers received");
        break;
      }
    }

    //Serial.println("reply was:");
    String line;
    int indexofvaultstr;
    int indexofvaultsearch = 0;
    while (httpsClient.available())
    {
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response

      if( isVaultFound == false )
      {
            indexofvaultstr = line.indexOf("vaultId", indexofvaultsearch);
        int indexofvault;
        int indexendofvault;
        do
        {
          if (indexofvaultstr != -1)
          {
            String vaultId;
          
            indexofvault = indexofvaultstr + 10;
            indexendofvault = line.indexOf("\"", indexofvault);
            vaultId = line.substring(indexofvault, indexendofvault);
            if( vaultId == Eeprom.GetDefichainVault() )
            {
              isVaultFound = true;
              Serial.print("Vault found! Index: ");
              Serial.println(indexendofvault);
            }
            else
            {
              indexofvaultsearch = indexendofvault;
              Serial.println("Wrong Vault! Index: ");
              Serial.println(indexendofvault);
              indexofvaultstr = line.indexOf("vaultId", indexofvaultsearch);
            }
          }
        }while( ( indexofvaultstr != -1 ) && ( isVaultFound == false ) );
      }

      if( isVaultFound == true )
      {
        if( isNextRatioDone == false )
        {
          int indexofnextratiostr = line.indexOf("informativeRatio", indexofvaultstr);
          int indexofnextratio;
          int indexendofnextratio;
          if (indexofnextratiostr != -1)
          {
            String nextRatio;
    
            indexofnextratio = indexofnextratiostr + 19;
            indexendofnextratio = line.indexOf("\"", indexofnextratio);
            nextRatio = line.substring(indexofnextratio, indexendofnextratio);
            Serial.print("Your vaults next ratio: ");
            Serial.println(nextRatio);
            *nextVaultRatio = nextRatio.toInt();
    
            isNextRatioDone = true;
          }
        }
  
        if( isCurrentRatioDone == false )
        {
          /* looks like everything comes in one line */
          int indexofcurrentratiostr = line.indexOf("collateralRatio", indexofvaultstr);
          int indexofcurrentratio;
          int indexendofcurrentratio;
          if (indexofcurrentratiostr != -1)
          {
            String currentRatio;

            indexofcurrentratio = indexofcurrentratiostr + 18;
            indexendofcurrentratio = line.indexOf("\"", indexofcurrentratio);
            currentRatio = line.substring(indexofcurrentratio, indexendofcurrentratio);
            Serial.print("Your vaults current ratio: ");
            Serial.println(currentRatio);
            *VaultRatio = currentRatio.toInt();
    
            isCurrentRatioDone = true;
          }
        }
  
        if( (isNextRatioDone == true) && (isCurrentRatioDone == true) )
        {
          break;
        }
      }

    }
    Serial.println("closing connection");

    httpsClient.stop();   
  }
  
}



void DefiChainAlarm_Http::GetDfiPrice(float* DfiPrice)
{
    // read DFI price
    bool isDfiPriceDone = false;

    const char *p_host = "ocean.defichain.com";
    const int p_httpsPort = 443;

    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

    httpsClient.setInsecure();

    Serial.println("HTTPS Connecting");
    int r = 0; //retry counter
    while ((!httpsClient.connect(p_host, p_httpsPort)) && (r < 30))
    {
      delay(100);
      Serial.print(".");
      r++;
    }
    if (r == 30)
    {
      Serial.println("Connection failed");
    }
    else
    {
      Serial.println("Connected to web");
      Serial.println("read DFI price");
      Serial.print("requesting URL: ");
      Serial.println(p_host);

      httpsClient.setTimeout(5000);

      httpsClient.print(String("GET ") + "/v0/mainnet/prices/DFI-USD" + " HTTP/1.1\r\n" +
                        "Host: " + p_host + "\r\n" +
                        "User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_1 like Mac OS X) AppleWebKit/603.1.30 (KHTML, like Gecko) Version/10.0 Mobile/14E304 Safari/602.1\r\n" +
                        "Connection: close\r\n\r\n");

      while (httpsClient.connected())
      {
        String line = httpsClient.readStringUntil('\n');
        //Serial.println(line); //Print response
        if (line == "\r")
        {
          //Serial.println("headers received");
          break;
        }
      }

      //Serial.println("reply was:");
      int indexofvaultstr;
      while (httpsClient.available())
      {
        String line = httpsClient.readStringUntil('\n');  //Read Line by Line
        //Serial.println(line); //Print response

        if( isDfiPriceDone == false )
        {
          int indexofpricestr = line.indexOf("amount", 0);
          int indexofprice;
          int indexendofprice;
          if (indexofpricestr != -1)
          {
            String lDfiPrice;
    
            indexofprice = indexofpricestr + 9;
            indexendofprice = line.indexOf("\"", indexofprice);
            lDfiPrice = line.substring(indexofprice, indexendofprice);
            Serial.print("DFI price: ");
            Serial.println(lDfiPrice);
            *DfiPrice = lDfiPrice.toFloat();
    
            isDfiPriceDone = true;
          }
        }
        
        if( isDfiPriceDone == true )
        {
          break;
        }

      }
      Serial.println("closing connection");
      httpsClient.stop();
    }
}

void DefiChainAlarm_Http::GetDusdFee(float* DusdFee)
{
    // read DUSD fee
    bool isDusdFeeDone = false;

    const char *p_host = "ocean.defichain.com";
    const int p_httpsPort = 443;

    WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

    httpsClient.setInsecure();

    Serial.println("HTTPS Connecting");
    int r = 0; //retry counter
    while ((!httpsClient.connect(p_host, p_httpsPort)) && (r < 30))
    {
      delay(100);
      Serial.print(".");
      r++;
    }
    if (r == 30)
    {
      Serial.println("Connection failed");
    }
    else
    {
      Serial.println("Connected to web");
      Serial.println("read DUSD fee");
      Serial.print("requesting URL: ");
      Serial.println(p_host);

      httpsClient.setTimeout(5000);

      httpsClient.print(String("GET ") + "/v0/mainnet/poolpairs/17" + " HTTP/1.1\r\n" +
                        "Host: " + p_host + "\r\n" +
                        "User-Agent: Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_1 like Mac OS X) AppleWebKit/603.1.30 (KHTML, like Gecko) Version/10.0 Mobile/14E304 Safari/602.1\r\n" +
                        "Connection: close\r\n\r\n");

      while (httpsClient.connected())
      {
        String line = httpsClient.readStringUntil('\n');
        //Serial.println(line); //Print response
        if (line == "\r")
        {
          //Serial.println("headers received");
          break;
        }
      }

      //Serial.println("reply was:");
      int indexofvaultstr;
      while (httpsClient.available())
      {
        String line = httpsClient.readStringUntil('\n');  //Read Line by Line
        //Serial.println(line); //Print response
        
        if( isDusdFeeDone == false )
        {
          int indexoffeestr = line.indexOf("inPct", 0);
          int indexoffee;
          int indexendoffee;
          if (indexoffeestr != -1)
          {
            String lDusdFee;
    
            indexoffee = indexoffeestr + 8;
            indexendoffee = line.indexOf("\"", indexoffee);
            lDusdFee = line.substring(indexoffee, indexendoffee);
            Serial.print("DUSD fee: ");
            Serial.println(lDusdFee);
            *DusdFee = lDusdFee.toFloat() * 100;
    
            isDusdFeeDone = true;
          }
        }
        
        if( isDusdFeeDone == true )
        {
          break;
        }

      }
      Serial.println("closing connection");
      httpsClient.stop();
    }
}
