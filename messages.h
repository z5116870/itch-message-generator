#pragma once
#include <cstdint>
#include <chrono>
#include <vector>
#include "helper.h"

// ------------------
// CONSTANTS
// ------------------

// MESSAGE TYPES
#define ADD_ORDER 'A'
#define TRADE 'P'
#define ORDER_EXECUTED 'E'
#define SYSTEM_EVENT 'S'
#define ORDER_CANCELLED 'C'

// INDICATORS
#define MARKET_OPEN 'O'
#define MARKET_CLOSE 'C'
#define BUY_ORDER 'B'
#define SELL_ORDER 'S'
#define IS_PRINTABLE 'Y'
#define IS_NOT_PRINTABLE 'N'

// ------------------
// ITCH MESSAGE TYPES
// ------------------
// BASE MESSAGE CLASS
class Message {
public:
    char        m_messageType;
    uint64_t    m_timestamp;          // Real value is 6 bytes, but we use 8 for ease of processing
public:
    Message(char messageType, uint64_t timestamp): m_messageType(messageType), m_timestamp(timestamp){};
    Message() = default;
    virtual uint8_t serialize();
};
    

// BASE ORDERMESSAGE CLASS
class OrderMessage : public Message {
public:
    uint64_t m_orderRefNumber;
    OrderMessage(char messageType, uint64_t timestamp): Message(messageType, timestamp) {
        setRandomFromVec(m_orderRefNumber, orderIds);
    };
    OrderMessage() = default;
};

// TRADE MESSAGE
// 'A' messageType means its an AddOrder message, 'P' means Trade message
class TradeMessage : public OrderMessage {
public:
    char        m_buySellIndicator;
    uint32_t    m_shares;
    char        m_stock[8];
    uint32_t    m_price;
public:
    TradeMessage();
};

// OTHER ITCH MESSAGES
class OrderExecutedMessage : public OrderMessage {
public:
    uint32_t m_executedShares;
    OrderExecutedMessage(): OrderMessage(ORDER_EXECUTED, getNanoSecondsSinceMidnight()), m_executedShares(ranInt32()){};
};

struct OrderExecutedWithPriceMessage : public OrderMessage {
public:
    uint32_t m_executedShares;
    char m_printable;
    uint32_t m_executionPrice;
    OrderExecutedWithPriceMessage();
};

struct SystemEventMessage : public Message {
public:
    char m_eventCode;
    SystemEventMessage(): Message(SYSTEM_EVENT, getNanoSecondsSinceMidnight()), m_eventCode(charRndmzr(MARKET_OPEN, MARKET_CLOSE)){};
};

struct OrderCancelMessage : public OrderMessage {
public:
    uint32_t m_cancelledShares;
    OrderCancelMessage(): OrderMessage(ORDER_CANCELLED, getNanoSecondsSinceMidnight()), m_cancelledShares(ranInt32()){};
};

// MESSAGE OVERLOADS
std::ostream &operator<<(std::ostream &s, TradeMessage &t);