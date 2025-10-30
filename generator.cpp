// RANDOMLY GENERATE ITCH MESSAGE
#include <memory>
#include <iostream>
#include <messages.h>
#define NUM_MESSAGES 6

// COMPILE TIME COMPUTATIONS
constexpr size_t MAX_SIZE = std::max({
    sizeof(TradeMessage),
    sizeof(OrderExecutedMessage),
    sizeof(OrderExecutedWithPriceMessage),
    sizeof(SystemEventMessage),
    sizeof(OrderCancelMessage)
});

constexpr size_t MAX_ALIGN = std::max({
    alignof(TradeMessage),
    alignof(OrderExecutedMessage),
    alignof(OrderExecutedWithPriceMessage),
    alignof(SystemEventMessage),
    alignof(OrderCancelMessage)
});

alignas(MAX_ALIGN)
std::byte shared_buffer[MAX_SIZE];

auto makeTrade = []() -> void* {return new (shared_buffer) TradeMessage();};
auto makeOrder = []() -> void* {return new (shared_buffer) OrderExecutedMessage();};
auto makeOrderPrice = []() -> void* {return new (shared_buffer) OrderExecutedWithPriceMessage();};
auto makeSystemEvent = []() -> void* {return new (shared_buffer) SystemEventMessage();};
auto makeOrderCancel = []() -> void* {return new (shared_buffer) OrderCancelMessage();};

constexpr std::array<void* (*)(), NUM_MESSAGES> messageConstructors = {
    makeTrade,
    makeOrder,
    makeOrderPrice,

}
template<typename T>
T generateMessage() {

}