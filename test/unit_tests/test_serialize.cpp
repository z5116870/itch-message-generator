// Test the serialization of all Message types
#include <iostream>
#include <cstring>
#include <cstdint>
#include "../../src/messages.h"
#include "../../src/helper.h"

// Simple test macros
#define TEST_CASE(name)  std::cout << "[TEST]: " << name << " ... "
#define PASS()           std::cout << "**PASS**\n"
#define FAIL(msg)        do { std::cerr << "!!FAIL!!: " << msg << std::endl; std::exit(1); } while(0)

// Helper to compare multi-byte integers in network byte order
bool compareNetworkOrder32(const uint8_t* buf, const uint32_t &value) {
    uint32_t net;
    std::memcpy(&net, buf, sizeof(net));
    // convert the serialized value back to host order and compare it
    net = ntohl(net);
    return net == value;
}

// 1. TradeMessage
void testTradeMessageSerialization() {
    TEST_CASE("TradeMessage serialization round-trip");

    // Randomize a TradeMessage
    TradeMessage msg;
    alignas(64) uint8_t buf[256] = {0};
    size_t len = msg.implSerialize(buf);

    if (len == 0) FAIL("Serialized length is 0");

    // Check first byte is message type
    if (buf[0] != msg.m_messageType) FAIL("Message type mismatch");

    // Check timestamp non-zero
    uint64_t timestamp;
    std::memcpy(&timestamp, buf + 1, sizeof(uint64_t));
    if (timestamp == 0) FAIL("Timestamp is zero");

    // Check the serialized value is equal to the struct values 
    if (!compareNetworkOrder32(buf + 20, msg.m_shares)) FAIL("Shares mismatch");

    PASS();
}

// 2. OrderExecutedMessage
void testOrderExecutedMessageSerialization() {
    TEST_CASE("OrderExecutedMessage serialization round-trip");

    OrderExecutedMessage msg;
    alignas(64) uint8_t buf[256] = {0};
    size_t len = msg.implSerialize(buf);

    if (len == 0) FAIL("Serialized length is 0");

    if (buf[0] != msg.m_messageType) FAIL("Message type mismatch");

    // Executed shares check
    if (!compareNetworkOrder32(buf + 19, msg.m_executedShares))
        FAIL("Executed shares mismatch");

    PASS();
}

// 3. OrderExecutedWithPrice
void testOrderExecutedWithPriceMessageSerialization() {
    TEST_CASE("OrderExecutedWithPriceMessage serialization round-trip");

    OrderExecutedWithPriceMessage msg;
    alignas(64) uint8_t buf[256] = {0};
    size_t len = msg.implSerialize(buf);

    if (len == 0) FAIL("Serialized length is 0");
    if (buf[0] != msg.m_messageType) FAIL("Message type mismatch");

    if (!compareNetworkOrder32(buf + 19, msg.m_executedShares))
        FAIL("Executed shares mismatch");
    if (!compareNetworkOrder32(buf + 24, msg.m_executionPrice))
        FAIL("Execution price mismatch");

    PASS();
}

// 4. SystemEventMessage
void testSystemEventMessageSerialization() {
    TEST_CASE("SystemEventMessage serialization round-trip");

    SystemEventMessage msg;
    alignas(64) uint8_t buf[256] = {0};
    size_t len = msg.implSerialize(buf);

    if (len == 0) FAIL("Serialized length is 0");
    if (buf[0] != msg.m_messageType) FAIL("Message type mismatch");
    if (buf[11] != msg.m_eventCode) FAIL("Event code mismatch");

    PASS();
}

// 5. OrderCancelMessage
void testOrderCancelMessageSerialization() {
    TEST_CASE("OrderCancelMessage serialization round-trip");

    OrderCancelMessage msg;
    alignas(64) uint8_t buf[256] = {0};
    size_t len = msg.implSerialize(buf);

    if (len == 0) FAIL("Serialized length is 0");
    if (buf[0] != msg.m_messageType) FAIL("Message type mismatch");

    if (!compareNetworkOrder32(buf + 19, msg.m_cancelledShares))
        FAIL("Cancelled shares mismatch");

    PASS();
}

int main() {
    std::cout << "=== ITCH MESSAGE GENERATOR: Serialization Unit Tests ===\n";
    testTradeMessageSerialization();
    testOrderExecutedMessageSerialization();
    testOrderExecutedWithPriceMessageSerialization();
    testSystemEventMessageSerialization();
    testOrderCancelMessageSerialization();
    std::cout << "\nAll serialization tests completed successfully.\n";
}
