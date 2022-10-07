#include "Arduino.h"
#include "DefiChainAlarm_Screen.h"

#include "epd_driver.h"
#include "font_TitilliumWeb_16.h"
#include "font_TitilliumWeb_18.h"
#include "font_TitilliumWeb_20.h"
#include "font_TitilliumWeb_26.h"
#include "font_TitilliumWeb_SemiBold_26.h"
#include "font_TitilliumWeb_SemiBold_50.h"
#include "font_TitilliumWeb_SemiBold_52.h"
#include "DefichainLogo.h"
#include <WiFi.h>
#include <time.h>


DefiChainAlarm_Screen::DefiChainAlarm_Screen()
{
  time_t now;
  tm tm;
  int cursor_loading_x = 10;
  int cursor_loading_y = 45;
  
  _framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!_framebuffer) {
      Serial.println("alloc memory failed !!!");
      while (1);
  }

  epd_init();

  memset(_framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
  writeln((GFXfont *)&TitilliumWeb_18, "loading...", &cursor_loading_x, &cursor_loading_y, _framebuffer); 
  epd_poweron();
  epd_clear();
  epd_draw_grayscale_image(epd_full_screen(), _framebuffer);
  epd_poweroff_all();

  memset(_SystemMessages, 0x00, NUMOF_SYSTEM_MSG * NUMOF_SYSTEM_MSG_CHARS);

  //init and get the time
  Serial.print("Initializing time from compiler tools ");
  configTime(1*3600, 0, "europe.pool.ntp.org");
  do{
    Serial.print(".");
    time(&now);
    localtime_r(&now, &tm);
    delay(500);
  }while(tm.tm_year < 122);
  Serial.println("ok");
}

void DefiChainAlarm_Screen::UpdateScreen(int VaultRatio, int nextVaultRatio, int VaultLimit, float DusdFee, float DfiPrice)
{
  /* copy parameters to internal variables */
  
  _OldVaultRatio = VaultRatio;
  _OldNextVaultRatio = nextVaultRatio;
  _OldVaultLimit = VaultLimit;
  _OldDusdFee = DusdFee;
  _OldDfiPrice = DfiPrice;

  
  /* *********************** Layout ************************ */
  
  //VAULT
  int cursor_vault_text_x = 20;
  int cursor_vault_text_y = 120;

  int cursor_vault_ratio_value_text_x = cursor_vault_text_x;
  int cursor_vault_ratio_value_text_y = cursor_vault_text_y + 95;
  
  int cursor_vault_nextratio_value_text_x = cursor_vault_ratio_value_text_x + 200;
  int cursor_vault_nextratio_value_text_y = cursor_vault_text_y + 55;
  
  int cursor_vault_limit_value_text_x = cursor_vault_ratio_value_text_x + 200;
  int cursor_vault_limit_value_text_y = cursor_vault_nextratio_value_text_y + 50;
  
  
  //DUSD FEE
  int cursor_dusd_text_x = 400;
  int cursor_dusd_text_y = 120;
  
  int cursor_dusd_fee_value_text_x = cursor_dusd_text_x;
  int cursor_dusd_fee_value_text_y = cursor_dusd_text_y + 55;
  
  
  //DFI PRICE
  int cursor_dfi_text_x = 680;
  int cursor_dfi_text_y = 120;
  
  int cursor_dfi_price_value_text_x = cursor_dfi_text_x;
  int cursor_dfi_price_value_text_y = cursor_dfi_text_y + 55;
  
  
  //system monitor
  int cursor_system_text_x = 20;
  int cursor_system_text_y = 300;
  
  int cursor_system_message_x_init = 40;
  int cursor_system_message_x = cursor_system_message_x_init;
  int cursor_system_message_y = 350;


  //wifi status bar
  int cursor_wifi_x = 10;
  int cursor_wifi_y = 45;


  //bgnd image
  Rect_t area_logo = {
        .x = 560,
        .y = 190,
        .width = DefichainLogo_width,
        .height =  DefichainLogo_height
    };
  
  char buf[40];
  
  memset(_framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
  
  epd_draw_hline( 10, 54, EPD_WIDTH-20, 0, _framebuffer );
  epd_draw_hline( 10, 55, EPD_WIDTH-20, 0, _framebuffer );
  epd_draw_hline( 10, 56, EPD_WIDTH-20, 0, _framebuffer );

  if (WiFi.status() == WL_CONNECTED) 
  {
    WiFi.SSID().toCharArray(buf, 40);
    strcat(buf," : ");
    strcat(buf,WiFi.localIP().toString().c_str());
  }
  else
  {
    strcpy(buf,"disconnected...");
  }

  //bgnd logo
  epd_copy_to_framebuffer(area_logo, (uint8_t *) DefichainLogo_data, _framebuffer);

  //infobar
  writeln((GFXfont *)&TitilliumWeb_18, buf, &cursor_wifi_x, &cursor_wifi_y, _framebuffer);  

  //VAULT
  writeln((GFXfont *)&TitilliumWeb_18, "vault [%]", &cursor_vault_text_x, &cursor_vault_text_y, _framebuffer);
  writeln((GFXfont *)&TitilliumWeb_SemiBold_52, String(VaultRatio).c_str(), &cursor_vault_ratio_value_text_x, &cursor_vault_ratio_value_text_y, _framebuffer);
  writeln((GFXfont *)&TitilliumWeb_SemiBold_26, String(nextVaultRatio).c_str(), &cursor_vault_nextratio_value_text_x, &cursor_vault_nextratio_value_text_y, _framebuffer);
  writeln((GFXfont *)&TitilliumWeb_SemiBold_26, String(VaultLimit).c_str(), &cursor_vault_limit_value_text_x, &cursor_vault_limit_value_text_y, _framebuffer);

  //DUSD FEE
  writeln((GFXfont *)&TitilliumWeb_18, "dUSD fee [%]", &cursor_dusd_text_x, &cursor_dusd_text_y, _framebuffer);
  writeln((GFXfont *)&TitilliumWeb_SemiBold_26, String(DusdFee).c_str(), &cursor_dusd_fee_value_text_x, &cursor_dusd_fee_value_text_y, _framebuffer);
  
  //DFI PRICE
  writeln((GFXfont *)&TitilliumWeb_18, "DFI price [USD]", &cursor_dfi_text_x, &cursor_dfi_text_y, _framebuffer);
  writeln((GFXfont *)&TitilliumWeb_SemiBold_26, String(DfiPrice).c_str(), &cursor_dfi_price_value_text_x, &cursor_dfi_price_value_text_y, _framebuffer);
  
  //system monitor
  writeln((GFXfont *)&TitilliumWeb_18, "system", &cursor_system_text_x, &cursor_system_text_y, _framebuffer);

  for(int i=0; i<NUMOF_SYSTEM_MSG; i++)
  {
    writeln((GFXfont *)&TitilliumWeb_16, _SystemMessages[i], &cursor_system_message_x, &cursor_system_message_y, _framebuffer);  
    cursor_system_message_x = cursor_system_message_x_init;
    cursor_system_message_y += 50;
  }

  epd_poweron();
  epd_clear();
  epd_draw_grayscale_image(epd_full_screen(), _framebuffer);
  //epd_poweroff();
  epd_poweroff_all();

}

void DefiChainAlarm_Screen::UpdateScreenMessages(void)
{
  UpdateScreen(_OldVaultRatio, _OldNextVaultRatio, _OldVaultLimit, _OldDusdFee, _OldDfiPrice);
}

void DefiChainAlarm_Screen::AddSystemMessage(const char* Message)
{ 
  String localMessage;
  String sMessage;
  char localSystemMessage[NUMOF_SYSTEM_MSG_CHARS];
  static int localIndex = 0;

  time_t now;                         // this is the epoch
  tm tm;                              // the structure tm holds time information in a more convient way

  sMessage = String(Message);
  
  if ( sMessage.length() > (NUMOF_SYSTEM_MSG_CHARS - 12) )
  {
    localMessage = sMessage.substring(0, (NUMOF_SYSTEM_MSG_CHARS - 12));
  }
  else
  {
    localMessage = sMessage;
  }

  // get timestamp
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time

  // prepare system message string
  if(tm.tm_isdst == 1)
  {
    sprintf(localSystemMessage, "[%02d:%02d:%02d] ", (tm.tm_hour+0), tm.tm_min, tm.tm_sec);
  }
  else
  {
    sprintf(localSystemMessage, "[%02d:%02d:%02d] ", (tm.tm_hour+1), tm.tm_min, tm.tm_sec);
  }
  strcat(localSystemMessage, localMessage.c_str()); 

  /* variant 2 : last message last */
  if( localIndex < NUMOF_SYSTEM_MSG )
  {
    memcpy(_SystemMessages[localIndex], localSystemMessage, NUMOF_SYSTEM_MSG_CHARS);
    localIndex++;
  }
  else
  {
    for(int i=1; i<NUMOF_SYSTEM_MSG; i++)
    {
      memcpy(_SystemMessages[i-1], _SystemMessages[i], NUMOF_SYSTEM_MSG_CHARS);
    } 
    memcpy(_SystemMessages[NUMOF_SYSTEM_MSG-1], localSystemMessage, NUMOF_SYSTEM_MSG_CHARS);
  }

}
