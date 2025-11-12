// RANDOMLY GENERATE ITCH MESSAGE
# pragma once
#include <iostream>
#include "messages.h"
//#define GENERATOR_LOG 1
#define NUM_MESSAGES 5
constexpr size_t MAX_ITCH_MSG_SIZE = 64;
// Retry buffer struct, for storing the message the triggered the user-space send buffer
// flush so it can be the first ITCH message written before the next sendto() call
struct RetryBuffer {
    uint8_t buf[MAX_ITCH_MSG_SIZE];
    size_t  size;
};

// Create array of void function pointers that write to a shared buffer.
// The shared buffer exists on the main stack, and is passed by pointer
// to each function, which writes a specific message type. The function pointer
// array is randomly indexed to pick which message to write next.
template<typename T>
inline size_t msgGen(uint8_t *buf ) {
    T t;
#ifdef GENERATOR_LOG
    std::cout << t << std::endl;
#endif
    return t.serialize(buf);
};

constexpr std::array<size_t(*)(uint8_t*), NUM_MESSAGES> generators = {
    &msgGen<TradeMessage>,
    &msgGen<OrderExecutedMessage>,
    &msgGen<OrderExecutedWithPriceMessage>,
    &msgGen<SystemEventMessage>,
    &msgGen<OrderCancelMessage>
};

ssize_t generateMessage(uint8_t *buf, RetryBuffer &retryBuf, const size_t &bytesRemaining);

