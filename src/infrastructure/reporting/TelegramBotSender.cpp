#include "infrastructure/reporting/TelegramBotSender.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <cstdio>
#include <cstring>
#include "infrastructure/reporting/TelegramRootCa.h"

namespace {
    BearSSL::X509List telegramRoot(TELEGRAM_ROOT_CA);

    String urlEncode(const char* input) {
        static const char HEX_DIGITS[] = "0123456789ABCDEF";
        String encoded;
        encoded.reserve(std::strlen(input) * 3);

        for (const unsigned char* cursor =
                 reinterpret_cast<const unsigned char*>(input); *cursor; cursor++) {
            const unsigned char value = *cursor;
            if ((value >= 'A' && value <= 'Z')
                || (value >= 'a' && value <= 'z')
                || (value >= '0' && value <= '9')
                || value == '-' || value == '_' || value == '.' || value == '~') {
                encoded += static_cast<char>(value);
            } else {
                encoded += '%';
                encoded += HEX_DIGITS[value >> 4];
                encoded += HEX_DIGITS[value & 0x0F];
            }
        }

        return encoded;
    }

    bool responseIsOk(const String& response) {
        const int keyAt = response.indexOf("\"ok\"");
        if (keyAt < 0) {
            return false;
        }

        int cursor = keyAt + 4;
        while (cursor < static_cast<int>(response.length()) && response[cursor] == ' ') {
            cursor++;
        }
        if (cursor >= static_cast<int>(response.length()) || response[cursor++] != ':') {
            return false;
        }
        while (cursor < static_cast<int>(response.length()) && response[cursor] == ' ') {
            cursor++;
        }
        return response.substring(cursor, cursor + 4) == "true";
    }
}

TelegramBotSender::TelegramBotSender(const char* token, const char* chatId)
    : token(token), chatId(chatId), transportCode(0) {}

int TelegramBotSender::lastTransportCode() const {
    return this->transportCode;
}

bool TelegramBotSender::send(
    int dateKey, int hour, int minute, float houseC, float outdoorC
) {
    if (this->token == nullptr || this->token[0] == '\0'
        || this->chatId == nullptr || this->chatId[0] == '\0') {
        this->transportCode = -100;
        return false;
    }

    char text[180];
    std::snprintf(
        text, sizeof(text),
        "Температура %02d.%02d.%04d о %02d:%02d\nУ будинку: %.2f °C\nНа вулиці: %.2f °C",
        dateKey % 100, (dateKey / 100) % 100, dateKey / 10000,
        hour, minute,
        static_cast<double>(houseC), static_cast<double>(outdoorC)
    );

    BearSSL::WiFiClientSecure client;
    client.setTrustAnchors(&telegramRoot);
    client.setTimeout(5000);

    HTTPClient request;
    request.setTimeout(5000);
    request.setReuse(false);
    const String url = String("https://api.telegram.org/bot")
        + this->token + "/sendMessage";
    if (!request.begin(client, url)) {
        this->transportCode = -101;
        return false;
    }

    request.addHeader("Content-Type", "application/x-www-form-urlencoded");
    const String body = String("chat_id=") + urlEncode(this->chatId)
        + "&text=" + urlEncode(text);
    const int status = request.POST(body);
    this->transportCode = status;
    const bool sent = status == HTTP_CODE_OK && responseIsOk(request.getString());
    request.end();
    return sent;
}
