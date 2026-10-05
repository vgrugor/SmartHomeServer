#ifndef TELEGRAM_BOT_SENDER_H
#define TELEGRAM_BOT_SENDER_H

#include "application/reporting/DailyTemperatureReporter.h"

class TelegramBotSender : public TemperatureMessageSender {
    public:
        TelegramBotSender(const char* token, const char* chatId);
        bool send(int dateKey, int hour, int minute, float houseC, float outdoorC) override;
        int lastTransportCode() const override;
        int lastTlsErrorCode() const override;
        const char* lastTlsErrorText() const override;

    private:
        const char* token;
        const char* chatId;
        int transportCode;
        int tlsErrorCode;
        char tlsErrorText[128];
};

#endif // TELEGRAM_BOT_SENDER_H
