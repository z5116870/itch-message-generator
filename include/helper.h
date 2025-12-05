#pragma once
#include <cstdint>
#include <vector>
#include <random>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h>
#include <random>


constexpr size_t TIMESTAMP_LEN = 6;
inline const std::vector<uint64_t> orderIds = {
    1001, 1002, 1003, 1004, 1005,
    1006, 1007, 1008, 1009, 1010
};

// GLOBAL SEQUENCE NUMBER
struct GlobalMessageState {
    inline static uint32_t sequenceNumber = 0;
};

static std::mt19937 gen(std::random_device{}());

inline const char* tickers[] = {
    "AAPL", "MSFT", "GOOGL", "BTC", "ETH", "GALA", "NVDA", "META", "VET", "XRP", "SOL", "DOGE"
};

uint64_t getNanoSecondsSinceMidnight();
char charRndmzr(char x, char y);
uint32_t ranInt32();
void setRandomTicker(char stock[8]);
void setRandomFromVec(uint64_t &x, const std::vector<uint64_t> &vec);
template<typename T>
inline void makeNetworkByteOrder(uint8_t *buf, size_t &offset, T val, size_t len = sizeof(T)) {
    // Stores bytes from MSB to LSB, regardless of architecture
    for(int i = len - 1; i >= 0; i--) 
        buf[offset++] = (val >> (i * 8)) & 0xFF;
    
}

inline void serialize32(uint8_t *buf, size_t &offset, const uint32_t &val) {
    uint32_t netByteOrder = htonl(val);
    memcpy(buf + offset, &netByteOrder, 4);
    offset += 4;
}

inline void serializeBaseMembers(uint8_t *buf, size_t &offset, const char &messageType, const uint64_t &timestamp) {
    // 1. Message Type
    buf[offset++] = messageType;

    // 2. Timestamp (big-endian must be preserved, 6 bytes)
    makeNetworkByteOrder(buf, offset, timestamp, TIMESTAMP_LEN);

    // 3. Sequence number
    serialize32(buf, offset, GlobalMessageState::sequenceNumber);
}