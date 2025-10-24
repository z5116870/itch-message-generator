#include <iostream>
#include "messages.h"
#include "helper.h"

// CONSTRUCTOR DEFINITIONS (THOSE TOO LONG TO FIT INLINE)
OrderExecutedWithPriceMessage::OrderExecutedWithPriceMessage(): OrderMessage('X', getNanoSecondsSinceMidnight()) {
    m_executedShares = ranInt32();
    m_printable = charRndmzr(IS_PRINTABLE, IS_NOT_PRINTABLE);
    m_executionPrice = ranInt32();
}

// TradeMessage class
TradeMessage::TradeMessage(char messageType): OrderMessage(messageType, getNanoSecondsSinceMidnight()) {
    m_buySellIndicator = charRndmzr(BUY_ORDER, SELL_ORDER);
    m_shares = ranInt32();
    setRandomTicker(m_stock);
    m_price = ranInt32();
};