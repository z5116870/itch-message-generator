#include <gtest/gtest.h>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>  // for ntohl
#include "messages.h"
#include "helper.h"

// ------------------------------------------------------------
// Tests for serialization of all Message types
// ------------------------------------------------------------

// Helper to compare multi-byte integers in network byte order
bool compareNetworkOrder32(const uint8_t* buf, const uint32_t &value) {
    uint32_t net;
    std::memcpy(&net, buf, sizeof(net));
    // convert the serialized value back to host order and compare it
    net = ntohl(net);
    return net == value;
}

// 1. TradeMessage
TEST(SerializationTest, TradeMessage) {
    // GIVEN a TradeMessage
    TradeMessage msg;
    alignas(64) uint8_t buf[256] = {0};

    // WHEN we serialize the message
    size_t len = msg.implSerialize(buf);

    // THEN the serialized length must be non-zero
    EXPECT_GT(len, 0) << "Serialized length is 0";

    // THEN the first byte must match the message type
    EXPECT_EQ(buf[0], msg.m_messageType) << "Message type mismatch";

    // THEN the timestamp must be non-zero
    uint64_t timestamp;
    std::memcpy(&timestamp, buf + 1, sizeof(uint64_t));
    EXPECT_NE(timestamp, 0) << "Timestamp is zero";

    // THEN the serialized shares value must match the struct value
    EXPECT_TRUE(compareNetworkOrder32(buf + 20, msg.m_shares)) << "Shares mismatch";
}

// 2. OrderExecutedMessage
TEST(SerializationTest, OrderExecutedMessage) {
    // GIVEN an OrderExecutedMessage
    OrderExecutedMessage msg;
    alignas(64) uint8_t buf[256] = {0};

    // WHEN we serialize the message
    size_t len = msg.implSerialize(buf);

    // THEN the serialized length must be non-zero
    EXPECT_GT(len, 0) << "Serialized length is 0";

    // THEN the first byte must match the message type
    EXPECT_EQ(buf[0], msg.m_messageType) << "Message type mismatch";

    // THEN the executed shares value must match
    EXPECT_TRUE(compareNetworkOrder32(buf + 19, msg.m_executedShares)) 
        << "Executed shares mismatch";
}

// 3. OrderExecutedWithPriceMessage
TEST(SerializationTest, OrderExecutedWithPriceMessage) {
    // GIVEN an OrderExecutedWithPriceMessage
    OrderExecutedWithPriceMessage msg;
    alignas(64) uint8_t buf[256] = {0};

    // WHEN we serialize the message
    size_t len = msg.implSerialize(buf);

    // THEN the serialized length must be non-zero
    EXPECT_GT(len, 0) << "Serialized length is 0";

    // THEN the first byte must match the message type
    EXPECT_EQ(buf[0], msg.m_messageType) << "Message type mismatch";

    // THEN executed shares and execution price must match the struct values
    EXPECT_TRUE(compareNetworkOrder32(buf + 19, msg.m_executedShares)) 
        << "Executed shares mismatch";
    EXPECT_TRUE(compareNetworkOrder32(buf + 24, msg.m_executionPrice)) 
        << "Execution price mismatch";
}

// 4. SystemEventMessage
TEST(SerializationTest, SystemEventMessage) {
    // GIVEN a SystemEventMessage
    SystemEventMessage msg;
    alignas(64) uint8_t buf[256] = {0};

    // WHEN we serialize the message
    size_t len = msg.implSerialize(buf);

    // THEN the serialized length must be non-zero
    EXPECT_GT(len, 0) << "Serialized length is 0";

    // THEN the first byte must match the message type
    EXPECT_EQ(buf[0], msg.m_messageType) << "Message type mismatch";

    // THEN the event code must match
    EXPECT_EQ(buf[11], msg.m_eventCode) << "Event code mismatch";
}

// 5. OrderCancelMessage
TEST(SerializationTest, OrderCancelMessage) {
    // GIVEN an OrderCancelMessage
    OrderCancelMessage msg;
    alignas(64) uint8_t buf[256] = {0};

    // WHEN we serialize the message
    size_t len = msg.implSerialize(buf);

    // THEN the serialized length must be non-zero
    EXPECT_GT(len, 0) << "Serialized length is 0";

    // THEN the first byte must match the message type
    EXPECT_EQ(buf[0], msg.m_messageType) << "Message type mismatch";

    // THEN the cancelled shares value must match
    EXPECT_TRUE(compareNetworkOrder32(buf + 19, msg.m_cancelledShares))
        << "Cancelled shares mismatch";
}