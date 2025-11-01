#include <iostream>
#include "messages.h"
#include "helper.h"
#define TIMESTAMP_LOG(s, t) s <<  "timestamp: " << t.m_timestamp << std::endl
#define TIMESTAMP_LEN 6

// CLASS METHOD DEFINITIONS
// CONSTRUCTORS
TradeMessage::TradeMessage(): OrderMessage(charRndmzr(ADD_ORDER, TRADE), getNanoSecondsSinceMidnight()) {
    m_buySellIndicator = charRndmzr(BUY_ORDER, SELL_ORDER);
    m_shares = ranInt32();
    setRandomTicker(m_stock);
    m_price = ranInt32();
};

OrderExecutedWithPriceMessage::OrderExecutedWithPriceMessage(): OrderMessage('X', getNanoSecondsSinceMidnight()) {
    m_executedShares = ranInt32();
    m_printable = charRndmzr(IS_PRINTABLE, IS_NOT_PRINTABLE);
    m_executionPrice = ranInt32();
}

// SERIALIZE FUNCTIONS
size_t TradeMessage::implSerialize(uint8_t *buf) const {
    size_t offset = 0;
    // 1. Message Type
    buf[offset++] = m_messageType;

    // 2. Timestamp (big-endian must be preserved, 6 bytes)
    makeNetworkByteOrder(buf, offset, m_timestamp, TIMESTAMP_LEN);

    // 3. Order Ref number (Use same function, but 8 bytes)
    makeNetworkByteOrder(buf, offset, m_orderRefNumber);

    // 4. Buy/Sell Indicator
    buf[offset++] = m_buySellIndicator;

    // 5. Shares Bought/Buying
    serialize32(buf, offset, m_shares);

    // 6. Stock Name
    memcpy(buf + offset, &m_stock, 8);
    offset += 8;

    // 7. Price 
    serialize32(buf, offset, m_price);

    // Return size (32 bytes)
    return offset;
}

size_t OrderExecutedMessage::implSerialize(uint8_t *buf) const {
    size_t offset = 0;
    buf[offset++] = m_messageType;
    makeNetworkByteOrder(buf, offset, m_timestamp, TIMESTAMP_LEN);
    makeNetworkByteOrder(buf, offset, m_orderRefNumber);

    // 4. Executed Shares
    serialize32(buf, offset, m_executedShares);

    return offset;
}

size_t OrderExecutedWithPriceMessage::implSerialize(uint8_t *buf) const {
    size_t offset = 0;
    buf[offset++] = m_messageType;
    makeNetworkByteOrder(buf, offset, m_timestamp, TIMESTAMP_LEN);
    makeNetworkByteOrder(buf, offset, m_orderRefNumber);

    // 4. Executed Shares
    serialize32(buf, offset, m_executedShares);

    // 5. Printable?
    buf[offset++] = m_printable;

    // 6. Execution Price
    serialize32(buf, offset, m_executionPrice);

    return offset;
}

size_t SystemEventMessage::implSerialize(uint8_t *buf) const {
    size_t offset = 0;
    buf[offset++] = m_messageType;
    makeNetworkByteOrder(buf, offset, m_timestamp, TIMESTAMP_LEN);

    // 4. Event Code
    buf[offset++] = m_eventCode;

    return offset;
}

size_t OrderCancelMessage::implSerialize(uint8_t *buf) const {
    size_t offset = 0;
    buf[offset++] = m_messageType;
    makeNetworkByteOrder(buf, offset, m_timestamp, TIMESTAMP_LEN);
    makeNetworkByteOrder(buf, offset, m_orderRefNumber);

    // 4. Cancelled Shares
    serialize32(buf, offset, m_cancelledShares);

    return offset;
}

// DEBUG FEATURE
// Operator overloads for <<
std::ostream &operator<<(std::ostream &s, TradeMessage &t) {
    TIMESTAMP_LOG(s, t);
    s << t.m_shares << " of " << t.m_stock;
    if (t.m_messageType == ADD_ORDER) {
        s <<  " order added ";
    } else s << " trade executed";
    return s;
}

std::ostream &operator<<(std::ostream &s, OrderExecutedMessage &t) {
    TIMESTAMP_LOG(s, t);
    s << t.m_executedShares << " shares from order no: " << t.m_orderRefNumber << " executed.";
    return s;
}

std::ostream &operator<<(std::ostream &s, OrderExecutedWithPriceMessage &t) {
    TIMESTAMP_LOG(s, t);
    s << t.m_executedShares << " shares from order no: " << t.m_orderRefNumber << " executed at price: " << t.m_executionPrice;
    return s;
}

std::ostream &operator<<(std::ostream &s, SystemEventMessage &t) {
    TIMESTAMP_LOG(s, t);
    if (t.m_eventCode == 'O') {
        s << "MARKET OPEN";
    } else if (t.m_eventCode == 'C'){
        s << "MARKET CLOSE";
    }
    return s;
}

std::ostream &operator<<(std::ostream &s, OrderCancelMessage &t) {
    TIMESTAMP_LOG(s, t);
    s << t.m_cancelledShares << " shares from order no: " << t.m_orderRefNumber << " cancelled.";
    return s;
}