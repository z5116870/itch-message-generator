#pragma once
#include <cstdint>
#include <vector>
inline const std::vector<uint64_t> orderIds = {
    1001, 1002, 1003, 1004, 1005,
    1006, 1007, 1008, 1009, 1010
};

inline const char* tickers[] = {
    "AAPL", "MSFT", "GOOGL", "BTC", "ETH", "GALA", "NVDA", "META", "VET", "XRP", "SOL", "DOGE"
};

uint64_t getNanoSecondsSinceMidnight();
char charRndmzr(char x, char y);
uint32_t ranInt32();
void setRandomTicker(char stock[8]);
void setRandomOrderId(uint64_t &x);