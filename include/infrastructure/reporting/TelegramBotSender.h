#ifndef TELEGRAM_BOT_SENDER_H
#define TELEGRAM_BOT_SENDER_H

#include "application/reporting/DailyTemperatureReporter.h"

class TelegramBotSender : public TemperatureMessageSender {
    public:
        TelegramBotSender(const char* token, const char* chatId);
        bool send(int dateKey, int hour, int minute, float houseC, float outdoorC) override;
        int lastTransportCode() const override;

    private:
        const char* token;
        const char* chatId;
        int transportCode;
};

#endif // TELEGRAM_BOT_SENDER_H
