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
    };

// BASE ORDERMESSAGE CLASS
class OrderMessage : public Message {
protected:
    uint64_t m_orderRefNumber;
public:
    OrderMessage(char messageType, uint64_t timestamp): Message(messageType, timestamp) {
        setRandomOrderId(m_orderRefNumber);
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
    TradeMessage(char messageType);
};

// OTHER ITCH MESSAGES
class OrderExecutedMessage : public OrderMessage {
    uint32_t m_executedShares;
public:
    OrderExecutedMessage(): OrderMessage(ORDER_EXECUTED, getNanoSecondsSinceMidnight()), m_executedShares(ranInt32()){};
};

struct OrderExecutedWithPriceMessage : public OrderMessage {
    uint32_t m_executedShares;
    char m_printable;
    uint32_t m_executionPrice;
public:
    OrderExecutedWithPriceMessage();
};

struct SystemEventMessage : public Message {
    char m_eventCode;
public:
    SystemEventMessage(): Message(SYSTEM_EVENT, getNanoSecondsSinceMidnight()), m_eventCode(charRndmzr(MARKET_OPEN, MARKET_CLOSE)){};
};

struct OrderCancelMessage : public OrderMessage {
    uint32_t m_cancelledShares;
public:
    OrderCancelMessage(): OrderMessage(ORDER_CANCELLED, getNanoSecondsSinceMidnight()), m_cancelledShares(ranInt32()){};
};