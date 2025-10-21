#include <generator.h>

AddOrderMessage generateAddOrderMessage() {
    AddOrderMessage m;
    m.messageType = 'A';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
};

OrderExecutedMessage generateOrderExecutedMessage() {
    OrderExecutedMessage m;
    m.messageType = 'E';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
}

OrderExecutedWithPriceMessage generateOrderExecutedWithPriceMessage() {
    OrderExecutedWithPriceMessage m;
    m.messageType = 'E';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
}

SystemEventMessage generateSystemEventMessage() {
    SystemEventMessage m;
    m.messageType = 'E';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
}

OrderCancelMessage generateOrderCancelMessage() {
    OrderCancelMessage m;
    m.messageType = 'E';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
}

TradeMessage generateTradeMessage() {
    TradeMessage m;
    m.messageType = 'E';
    m.timestamp = getNanoSecondsSinceMidnight();
    return m;
}

uint64_t getNanoSecondsSinceMidnight() {
        using namespace std::chrono;
    auto now = system_clock::now();
    // truncate the time value to store only days using "days" alias
    auto midnight = time_point_cast<days>(now);
    auto delta = now - midnight;
    return duration_cast<nanoseconds>(delta).count();
}