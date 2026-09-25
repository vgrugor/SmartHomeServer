#include "infrastructure/reporting/EepromSentDateStore.h"
#include <EEPROM.h>
#include <cstdint>

namespace {
    constexpr uint32_t MAGIC = 0x54454C45; // "TELE"
    constexpr int STORAGE_SIZE = 12;

    uint32_t readWord(int offset) {
        uint32_t value = 0;
        for (int i = 0; i < 4; ++i) {
            value |= static_cast<uint32_t>(EEPROM.read(offset + i)) << (i * 8);
        }
        return value;
    }

    void writeWord(int offset, uint32_t value) {
        for (int i = 0; i < 4; ++i) {
            EEPROM.write(offset + i, static_cast<uint8_t>(value >> (i * 8)));
        }
    }
}

bool EepromSentDateStore::load(int& dateKey) {
    EEPROM.begin(STORAGE_SIZE);
    if (EEPROM.getConstDataPtr() == nullptr) {
        return false;
    }

    const uint32_t magic = readWord(0);
    const uint32_t value = readWord(4);
    const uint32_t inverse = readWord(8);
    if (magic == 0xFFFFFFFF && value == 0xFFFFFFFF && inverse == 0xFFFFFFFF) {
        dateKey = 0;
        return true;
    }

    if (magic != MAGIC || inverse != ~value
        || value < 20240101 || value > 20991231) {
        return false;
    }

    dateKey = static_cast<int>(value);
    return true;
}

bool EepromSentDateStore::save(int dateKey) {
    if (dateKey < 20240101 || dateKey > 20991231) {
        return false;
    }

    const uint32_t value = static_cast<uint32_t>(dateKey);
    writeWord(0, MAGIC);
    writeWord(4, value);
    writeWord(8, ~value);
    return EEPROM.commit();
}
