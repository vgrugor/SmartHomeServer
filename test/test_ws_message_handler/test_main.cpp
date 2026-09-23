#include <cstring>
#include <unity.h>
#include "application/WsMessageHandler.h"

void setUp() {}
void tearDown() {}

void test_recognizes_get_values_command() {
    WsMessageHandler handler;
    const char command[] = "getValues";

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::GET_VALUES),
        static_cast<int>(handler.handle(command, sizeof(command) - 1))
    );
}

void test_rejects_unknown_command() {
    WsMessageHandler handler;
    const char command[] = "setValues";

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(command, sizeof(command) - 1))
    );
}

void test_rejects_partial_command() {
    WsMessageHandler handler;
    const char command[] = "getValue";

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(command, sizeof(command) - 1))
    );
}

void test_rejects_command_containing_get_values() {
    WsMessageHandler handler;
    const char prefixed[] = "prefix-getValues";
    const char suffixed[] = "getValues-extra";

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(prefixed, sizeof(prefixed) - 1))
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(suffixed, sizeof(suffixed) - 1))
    );
}

void test_rejects_empty_and_null_messages() {
    WsMessageHandler handler;

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle("", 0))
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(nullptr, 0))
    );
}

void test_rejects_overlong_message_without_reading_past_length() {
    WsMessageHandler handler;
    char message[WsMessageHandler::MAX_MESSAGE_LENGTH + 2];
    std::memset(message, 'a', sizeof(message));

    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(WsMessageType::UNKNOWN),
        static_cast<int>(handler.handle(message, sizeof(message)))
    );
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_recognizes_get_values_command);
    RUN_TEST(test_rejects_unknown_command);
    RUN_TEST(test_rejects_partial_command);
    RUN_TEST(test_rejects_command_containing_get_values);
    RUN_TEST(test_rejects_empty_and_null_messages);
    RUN_TEST(test_rejects_overlong_message_without_reading_past_length);
    return UNITY_END();
}
