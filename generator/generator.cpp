#include <random>
#include <cstring>
#include <generator.h>

AddOrderMessage generateAddOrderMessage() {
    AddOrderMessage m;
    m.messageType = 'A';
    m.timestamp = getNanoSecondsSinceMidnight();
    setRandomOrderId(&m.orderRefNumber);
    m.buySellIndicator = buySellCharRndmzr();
    m.shares = ranInt32();
    setRandomTicker(m.stock);
    m.price = ranInt32();
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

uint32_t ranInt32() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,UINT32_MAX);
    return dist(gen);
}

char buySellCharRndmzr() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,1);
    int bit = dist(gen);
    return bit ? 'B' : 'S';
}

void setRandomOrderId(uint64_t *x) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,orderIds.size() - 1);
    *x = dist(gen);
}

void setRandomTicker(char stock[8]) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,sizeof(tickers)/sizeof(tickers[0]) - 1);
    int index = dist(gen);

    // Use random number generator generate index, then copy value at index
    // into stock
    std::strncpy(stock, tickers[index], 8);

    // Set last byte to null terminator string
    stock[7] = '/0';
}