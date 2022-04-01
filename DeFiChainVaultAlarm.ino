
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot


#ifdef ESP8266
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif


/* ******************** configurtion ********************* */
// Replace with your network credentials
const char* p_ssid = "Skynet";
const char* p_password = "password";

// Initialize Telegram BOT
#define BOTtoken "1234567890:abcdefghijklmnopqrstuvwxyz1234567890"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "361094966"

const char *p_host = "defiscan.live";
const String p_link = "/vaults/abcdefghijklmnopqrstuvwxyz1234567890";
const int p_httpsPort = 443;

const int p_buzzer = 5; //GPIO pin number (remark: GPIO pin number may be different to uc port number)

volatile int p_VaultLimit = 185;


/* ****************** public variables ******************** */
WiFiClientSecure p_TelegramClient;
UniversalTelegramBot p_bot(BOTtoken, p_TelegramClient);

// Checks for new messages every 1 second.
int p_botRequestDelay = 1000;
unsigned long p_lastTimeBotRan;

bool p_ShallRefreshRatio = false;
bool p_ShallRefreshRatioNow = false;
bool p_SendRefreshedRatio = false;

int p_VaultRefreshDelay = 1000 * 60 * 30;
unsigned long p_LastVaultRefreshRan;

int p_VaultRefreshRetryDelay = 1000 * 60 * 5;
unsigned long p_LastVaultRefreshRetryRan;

int p_NextVaultRatio = -1;

bool p_ReadVaultLimit = false;


/* **************** function declarations **************** */
int getVaultStatusGET(void);
void handleNewMessages(int numNewMessages);
bool testVaultStatus(int nextVaultRatio);


/* ******************************************************* */

void setup()
{
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");               // get UTC time via NTP
  p_TelegramClient.setTrustAnchors(&cert);        // Add root certificate for api.telegram.org
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(p_ssid, p_password);

#ifdef ESP32
  p_TelegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
#endif
  Serial.println("");
  Serial.println("");

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi: ");
  Serial.println(p_ssid);

  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
}


void loop()
{
  // Telegram
  if (millis() > p_lastTimeBotRan + p_botRequestDelay)
  {
    int numNewMessages = p_bot.getUpdates(p_bot.last_message_received + 1);

    while (numNewMessages)
    {
      handleNewMessages(numNewMessages);
      numNewMessages = p_bot.getUpdates(p_bot.last_message_received + 1);
    }

    p_lastTimeBotRan = millis();
  }

  // Vault status
  if (millis() > p_LastVaultRefreshRan + p_VaultRefreshDelay)
  {
    p_NextVaultRatio = getVaultStatusGET();
    testVaultStatus(p_NextVaultRatio);
    if (p_SendRefreshedRatio == true)
    {
      p_bot.sendMessage(CHAT_ID, String(p_NextVaultRatio) + "%", "");
    }
    p_LastVaultRefreshRan = millis();
  }


  if ( p_ShallRefreshRatio != false )
  {
    if ( (millis() > p_LastVaultRefreshRetryRan + p_VaultRefreshRetryDelay) || (p_ShallRefreshRatioNow == true) )
    {
      bool wasHttpRequestOk;
      
      p_NextVaultRatio = getVaultStatusGET();

      wasHttpRequestOk = testVaultStatus(p_NextVaultRatio);
      
      if(p_ShallRefreshRatioNow == true)
      {
        if( wasHttpRequestOk )
        {
          p_bot.sendMessage(CHAT_ID, "Next vault status: " + String(p_NextVaultRatio) + "%", "");
          p_ShallRefreshRatio = false;
          p_ShallRefreshRatioNow = false;
        }
        else
        {
          p_ShallRefreshRatio = true;
          p_ShallRefreshRatioNow = false;
          p_LastVaultRefreshRetryRan = millis();
        }
      }
      else
      {
          p_ShallRefreshRatio = false;
          p_ShallRefreshRatioNow = false; 
      }
      
      testVaultStatus(p_NextVaultRatio);
      p_ShallRefreshRatio = false;
      p_ShallRefreshRatioNow = false;
    }
  }

}


int getVaultStatusGET(void)
{
  int retVal = -1;

  WiFiClientSecure httpsClient;    //Declare object of class WiFiClient

  Serial.println(p_host);

  httpsClient.setInsecure();

  Serial.print("HTTPS Connecting");
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

    String getData;

    Serial.print("requesting URL: ");
    Serial.println(p_host);

    httpsClient.setTimeout(5000);

    httpsClient.print(String("GET ") + p_link + " HTTP/1.1\r\n" +
                      "Host: " + p_host + "\r\n" +
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
    while (httpsClient.available())
    {
      line = httpsClient.readStringUntil('\n');  //Read Line by Line
      //Serial.println(line); //Print response

      int indexofnextratiostr = line.indexOf("Next ~", 0);
      int indexofnextratio;
      int indexendofnextratio;
      if (indexofnextratiostr != -1)
      {
        String nextRatio;

        indexofnextratio = indexofnextratiostr + 6;
        indexendofnextratio = line.indexOf("%", indexofnextratio);
        nextRatio = line.substring(indexofnextratio, indexendofnextratio);
        Serial.print("Your vaults next ratio: ");
        Serial.println(nextRatio);
        retVal = nextRatio.toInt();

        break;
      }

    }
    Serial.println("closing connection");

    httpsClient.stop();
  }

  return retVal;
}


//Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // String to search for
    String text_ratio = "Current ratio: ";

    // Chat id of the requester
    String chat_id = String(p_bot.messages[i].chat_id);
    if ( chat_id != CHAT_ID )
    {
      p_bot.sendMessage(chat_id, "I don't know you.", "");
      p_bot.sendMessage(chat_id, "Whatever...", "");
    }
    else
    {
      // Print the received message
      String text = p_bot.messages[i].text;
      Serial.println(text);

      String from_name = p_bot.messages[i].from_name;

      if( p_ReadVaultLimit == true )
      {
        int newVaultLimit;
        newVaultLimit = text.toInt();
        
        if(newVaultLimit!=0)
        {
          p_VaultLimit = newVaultLimit;
          p_ReadVaultLimit = false;
          p_bot.sendMessage(CHAT_ID, "New vault alarm limit stored.", "");
          testVaultStatus(p_VaultLimit);
        }
        else
        {
          p_ReadVaultLimit = false;
          p_bot.sendMessage(CHAT_ID, "Error: Not a number.", "");
        }
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
        welcome += "/sendkeyboard will send this commands as special keyboard \n";
        p_bot.sendMessage(CHAT_ID, welcome, "");
      }
      else if (text == "/buzzertest")
      {
        p_bot.sendMessage(CHAT_ID, "Buzzer activated...", "");
        pinMode(p_buzzer, OUTPUT);  
        analogWrite(p_buzzer, 10);
        delay(1000);
        analogWrite(p_buzzer, 0);
        pinMode(p_buzzer, INPUT);  
        p_bot.sendMessage(CHAT_ID, "Buzzer deactivated...", "");
      }
      else if (text == "/vault")
      {
        String vaultUrl;
        vaultUrl = "Your vault: ";
        vaultUrl += p_host;
        vaultUrl += p_link;
        p_bot.sendMessage(CHAT_ID, vaultUrl, "");
      }
      else if (text == "/refresh")
      {
        p_ShallRefreshRatio = true;
        p_ShallRefreshRatioNow = true;

        p_bot.sendMessage(CHAT_ID, "Just a second. I will check your vault status. (This may take some while in case site response is invalid)", "");
      }
      else if (text == "/protocol")
      {
        if ( p_SendRefreshedRatio == false )
        {
          p_SendRefreshedRatio = true;
          p_bot.sendMessage(CHAT_ID, "Protocol mode ON", "");
        }
        else
        {
          p_SendRefreshedRatio = false;
          p_bot.sendMessage(CHAT_ID, "Protocol mode OFF", "");
        }
      }
      else if (text == "/readlimit")
      {
        p_bot.sendMessage(CHAT_ID, "The currently configured vault alarm limit is " + String(p_VaultLimit) + "%", "");
      }
      else if (text == "/setlimit")
      {
        p_bot.sendMessage(CHAT_ID, "Please write new vault alarm limit.", "");
        p_ReadVaultLimit = true;
      }
      else if (text == "/sendkeyboard")
      {
        String keyboardJson = "[[\"/start\"],[\"/refresh\", \"/vault\"],[\"/buzzertest\", \"/protocol\"],[\"/readlimit\", \"/setlimit\"]]";
        p_bot.sendMessageWithReplyKeyboard(CHAT_ID, "Choose from one of the following options", "", keyboardJson, true);
      }
    }

  }
}

bool testVaultStatus(int nextVaultRatio)
{
  static int retryCounter = 0;
  bool retRatioValid = false;
  
  if (nextVaultRatio != -1)
  { 
    retRatioValid = true;
    
    if (nextVaultRatio < p_VaultLimit)
    {
      //ALARM
      pinMode(p_buzzer, OUTPUT);  
      analogWrite(p_buzzer, 10);
      p_bot.sendMessage(CHAT_ID, "ALARM! Your vault is at " + String(p_NextVaultRatio) + "%", "");
    }

    retryCounter = 0;
  }
  else if(nextVaultRatio == -1)
  { 
    //SOFT-ALARM
    //p_bot.sendMessage(CHAT_ID, "WARNING! Ratio not readable. Will try to read it again in 5min.", "");
    p_ShallRefreshRatio = true;
    p_LastVaultRefreshRetryRan = millis();

    if(retryCounter > 6)
    {
      //ALARM
      p_bot.sendMessage(CHAT_ID, "ALARM! Ratio not readable. Check internet connection.", "");
      pinMode(p_buzzer, OUTPUT);  
      analogWrite(p_buzzer, 10);
    }
    else
    {
      retryCounter++;
    }
    
  }
  else
  {
    //its fine
  }

  return retRatioValid;
}
