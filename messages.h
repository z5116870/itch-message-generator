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
#define ORDER_EXECUTED_WITH_PRICE 'X'
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
// Message and OrderMessage classes use static polymorphism to allow compile time resolving of 
// the serialize function -> much faster than virtual functions (no vtable, vptr or indirect
// function call overhead)
template<typename DerivedMessage>
class Message {
public:
    char                        m_messageType;
    uint64_t                    m_timestamp;          // We use 8 bytes here, but 6 bytes when serializing
public:
    Message(char messageType, uint64_t timestamp): m_messageType(messageType), m_timestamp(timestamp){
        // Increment the sequence number, then store it.
        ++GlobalMessageState::sequenceNumber;
    };
    Message() = default;
    size_t serialize(uint8_t *buf) const {
        return static_cast<const DerivedMessage*>(this)->implSerialize(buf);
    }
};

// BASE ORDERMESSAGE CLASS
template<typename DerivedMessage>
class OrderMessage : public Message<DerivedMessage> {
public:
    uint64_t m_orderRefNumber;
    OrderMessage(char messageType, uint64_t timestamp): Message<DerivedMessage>(messageType, timestamp) {
        setRandomFromVec(m_orderRefNumber, orderIds);
    };
    OrderMessage() = default;
};

// TRADE MESSAGE
// 'A' messageType means its an AddOrder message, 'P' means Trade message
class TradeMessage : public OrderMessage<TradeMessage> {
public:
    char        m_buySellIndicator;
    uint32_t    m_shares;
    char        m_stock[8];
    uint32_t    m_price;
public:
    TradeMessage();
    size_t implSerialize(uint8_t *buf) const;
};

// OTHER ITCH MESSAGES
class OrderExecutedMessage : public OrderMessage<OrderExecutedMessage> {
public:
    uint32_t m_executedShares;
    OrderExecutedMessage(): OrderMessage(ORDER_EXECUTED, getNanoSecondsSinceMidnight()), m_executedShares(ranInt32()){};
    size_t implSerialize(uint8_t *buf) const;
};

struct OrderExecutedWithPriceMessage : public OrderMessage<OrderExecutedWithPriceMessage> {
public:
    uint32_t m_executedShares;
    char m_printable;
    uint32_t m_executionPrice;
    OrderExecutedWithPriceMessage();
    size_t implSerialize(uint8_t *buf) const;
};

struct SystemEventMessage : public Message<SystemEventMessage>{
public:
    char m_eventCode;
    SystemEventMessage(): Message(SYSTEM_EVENT, getNanoSecondsSinceMidnight()), m_eventCode(charRndmzr(MARKET_OPEN, MARKET_CLOSE)){};
    size_t implSerialize(uint8_t *buf) const;
};

struct OrderCancelMessage : public OrderMessage<OrderCancelMessage> {
public:
    uint32_t m_cancelledShares;
    OrderCancelMessage(): OrderMessage(ORDER_CANCELLED, getNanoSecondsSinceMidnight()), m_cancelledShares(ranInt32()){};
    size_t implSerialize(uint8_t *buf) const;
};

// MESSAGE OVERLOADS
std::ostream &operator<<(std::ostream &s, TradeMessage &t);
std::ostream &operator<<(std::ostream &s, OrderExecutedMessage &t);
std::ostream &operator<<(std::ostream &s, OrderExecutedWithPriceMessage &t);
std::ostream &operator<<(std::ostream &s, SystemEventMessage &t);
std::ostream &operator<<(std::ostream &s, OrderCancelMessage &t);