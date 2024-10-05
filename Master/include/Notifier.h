#include <Arduino.h>
#include <AsyncTelegram2.h>
#include <WiFiClientSecure.h>
#include <TelegramCredentials.h>

#include "main.h"

#pragma once

#ifndef NOTIFIER_H
#define NOTIFIER_H
#define BOTtoken "5940402363:AAH9-eEQ6q2mT1jnP1OSTPakGW4V2AKf6do"

    class Notifier
    {
    private:
        WiFiClientSecure telegramClient;
        AsyncTelegram2 TelegramBot = AsyncTelegram2(telegramClient);

    public:
        Notifier();
        ~Notifier();

        bool begin();

        void loop();

        bool send(String Message);
        bool sendTo(String Message);
        bool sendToChannel(String Message);

        bool NotifyIndividuel = true;
        bool NotifyGroup = true;

        bool checkMessage = true;

    };

    extern Notifier Notifi;
#endif
