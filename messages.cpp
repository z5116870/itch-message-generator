#include <iostream>
#include "messages.h"
#include "helper.h"
#define SIZE_TIME_LOG(s, t) s << sizeof(t) << " bytes sent. " <<  "timestamp: " << t.m_timestamp << std::endl
// CONSTRUCTOR DEFINITIONS (THOSE TOO LONG TO FIT INLINE)
OrderExecutedWithPriceMessage::OrderExecutedWithPriceMessage(): OrderMessage('X', getNanoSecondsSinceMidnight()) {
    m_executedShares = ranInt32();
    m_printable = charRndmzr(IS_PRINTABLE, IS_NOT_PRINTABLE);
    m_executionPrice = ranInt32();
}

// TradeMessage class
TradeMessage::TradeMessage(): OrderMessage(charRndmzr(ADD_ORDER, TRADE), getNanoSecondsSinceMidnight()) {
    m_buySellIndicator = charRndmzr(BUY_ORDER, SELL_ORDER);
    m_shares = ranInt32();
    setRandomTicker(m_stock);
    m_price = ranInt32();
};

std::ostream &operator<<(std::ostream &s, TradeMessage &t) {
    SIZE_TIME_LOG(s, t);
    s << t.m_shares << " of " << t.m_stock;
    if (t.m_messageType == ADD_ORDER) {
        s <<  " order added ";
    } else s << " trade executed";
    return s << std::endl;
}

std::ostream &operator<<(std::ostream &s, OrderExecutedMessage &t) {
    SIZE_TIME_LOG(s, t);
    s << t.m_executedShares << " shares from order no: " << t.m_orderRefNumber << " executed.";
    return s << std::endl;
}

std::ostream &operator<<(std::ostream &s, OrderExecutedWithPriceMessage &t) {
    SIZE_TIME_LOG(s, t);
    s << t.m_executedShares << " shares from order no: " << t.m_orderRefNumber << " executed at price: " << t.m_executionPrice;
    return s << std::endl;
}

std::ostream &operator<<(std::ostream &s, SystemEventMessage &t) {
    SIZE_TIME_LOG(s, t);
    if (t.m_eventCode = 'O') {
        s << "MARKET OPEN";
    } else if (t.m_eventCode = 'C'){
        s << "MARKET CLOSE";
    }
    return s << std::endl;
}

std::ostream &operator<<(std::ostream &s, OrderCancelMessage &t) {
        SIZE_TIME_LOG(s, t);
    s << t.m_cancelledShares << " shares from order no: " << t.m_orderRefNumber << " cancelled.";
    return s << std::endl;
}