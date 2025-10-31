// RANDOMLY GENERATE ITCH MESSAGE
#include <memory>
#include <iostream>
#include <random>
#include "messages.h"
#define NUM_MESSAGES 5

// Create array of void function pointers that write to a shared buffer.
// The shared buffer exists on the main stack, and is passed by pointer
// to each function, which writes a specific message type. The function pointer
// array is randomly indexed to pick which message to write next.
template<typename T>
size_t msgGen(uint8_t *buf ) {
    T t;
    std::cout << t << std::endl;
    return t.serialize(buf);
};

constexpr std::array<size_t(*)(uint8_t*), NUM_MESSAGES> generators = {
    &msgGen<TradeMessage>,
    &msgGen<OrderExecutedMessage>,
    &msgGen<OrderExecutedWithPriceMessage>,
    &msgGen<SystemEventMessage>,
    &msgGen<OrderCancelMessage>
};

size_t generateMessage(uint8_t *buf);