#include "Notifier.h"

Notifier::Notifier()
{
}

Notifier::~Notifier()
{
}
bool Notifier::begin()
{
  #if false
    telegramClient.setCACert(telegram_cert);
    TelegramBot.setTelegramToken(BOTtoken);
    TelegramBot.setUpdateTime(4000);

    Notifi.NotifyIndividuel = Prefs.getBool("Notif",true);
    Notifi.NotifyGroup = Prefs.getBool("NotifGroup",true);
    return TelegramBot.begin();
    #else
      return true;
    #endif
}

void Notifier::loop()
{
  
  #if false
    if (!checkMessage)
    {
        return;
    }
    TBMessage msg;
    if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WiFiMode_t::WIFI_MODE_STA)
    {
        return;
    }
    if (TelegramBot.getNewMessage(msg)) {
    Serial.printf("[telegram] %s\n",msg.text);
    switch (msg.messageType)
    {
      case MessageText:
        if (msg.text.equalsIgnoreCase("/html"))
        {
          TelegramBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML);
          TelegramBot.sendMessage(msg,"<a href=\"http://www.example.com/\">inline URL</a>");
        }
        // check if is show keyboard command
        if (msg.text.equalsIgnoreCase("/reply_keyboard")) {
          // the user is asking to show the reply keyboard --> show it
          //TelegramBot.sendMessage(msg, "This is reply keyboard:", myreplykbd);
          //iskeyboardactive = true;
        }
        else if (msg.text.equalsIgnoreCase("/inline_keyboard")) {
          //TelegramBot.sendMessage(msg, "This is inline keyboard:", myinlinekbd);
        }

        // check if the reply keyboard is active
        // else if (iskeyboardactive) {
        //   // is active -> manage the text messages sent by pressing the reply keyboard buttons
        //   if (msg.text.equalsIgnoreCase("/hide_keyboard")) {
        //     // sent the "hide keyboard" message --> hide the reply keyboard
        //     TelegramBot.removeReplyKeyboard(msg, "Reply keyboard removed");
        //     iskeyboardactive = false;
        //   } else {
        //     // print every others messages received
        //     TelegramBot.sendMessage(msg, msg.text);
        //   }
        // }
      break;

      case MessageQuery:
        if (msg.callbackQueryData.equalsIgnoreCase("test"))
        {
          TelegramBot.endQuery(msg,"test,true");
        }
        
        
      break;
    }
    
  
  }
  #endif
    
}

bool Notifier::send(String Message)
{
    sendTo(Message);
    sendToChannel(Message);
    
    
    
    return false;
}

bool Notifier::sendTo(String Message)
{
  #if false
    if (NotifyIndividuel)
    {
      TelegramBot.sendTo(CHAT_ID,Message);
    }
    return false;
    #else 
    return true;
  #endif
}

bool Notifier::sendToChannel(String Message)
{
  #if false
    if (NotifyGroup)
    {
      TelegramBot.sendToChannel(GROUP_ID,Message,false);
        
    }
    return false;
    #else 
    return true;
  #endif
}

Notifier Notifi;