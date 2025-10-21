#include <cstdint>
#include <chrono>

// ITCH MESSAGE TYPES
struct AddOrderMessage {
    char        messageType;
    uint64_t    timestamp;          // Real value is 6 bytes, but we use 8 for ease of processing
    uint64_t    orderRefNumber;
    char        buySellIndicator;
    uint32_t    shares;
    char        stock[8];
    uint32_t    price;
};

struct OrderExecutedMessage {
    char messageType;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
};

struct OrderExecutedWithPriceMessage {
    char messageType;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    uint32_t executedShares;
    uint64_t matchNumber;
    char printable;
    uint32_t executionPrice;
};

struct SystemEventMessage {
    char messageType;
    uint64_t timestamp;
    char eventCode;
};

struct OrderCancelMessage {
    char messageType;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    uint32_t cancelledShares;
};

struct TradeMessage {
    char messageType;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    char buySellIndicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t matchNumber;
};

// ITCH MESSAGE GENERATOR HELPER FUNCTIONS
AddOrderMessage generateAddOrderMessage();
OrderExecutedMessage generateOrderExecutedMessage();
OrderExecutedWithPriceMessage generateOrderExecutedWithPriceMessage();
SystemEventMessage generateSystemEventMessage();
OrderCancelMessage generateOrderCancelMessage();
TradeMessage generateTradeMessage();

// OTHER HELPER FUNCTIONS
uint64_t getNanoSecondsSinceMidnight();