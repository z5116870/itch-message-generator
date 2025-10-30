#include "helper.h"
#include <random>
#include <cstring>
#include <chrono>
#include <iostream>

// HELPER FUNCTIONS
uint64_t getNanoSecondsSinceMidnight() {
    using namespace std::chrono;
    auto now = system_clock::now();
    // truncate the time value to store only days using "days" alias
    auto midnight = floor<days>(now);
    auto delta = now - midnight;
    uint64_t nano = duration_cast<nanoseconds>(delta).count();
    return nano;
}

uint32_t ranInt32() {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0,UINT32_MAX-1);
    return dist(gen);
}

char charRndmzr(char x, char y) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,1);
    int bit = dist(gen);
    return bit ? x : y;
}

void setRandomFromVec(uint64_t &x, const std::vector<uint64_t> &vec) {
    std::mt19937 gen(std::random_device{}());
    if (!vec.size()) {
        perror("Vector is empty.");
        exit(EXIT_FAILURE);
    };
    std::uniform_int_distribution<int> dist(0,vec.size() - 1);
    int idx = dist(gen);
    x = vec[dist(gen)];
}

void setRandomTicker(char stock[8]) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0,sizeof(tickers)/sizeof(tickers[0]) - 1);
    int index = dist(gen);

    // Use random number generator generate index, then copy value at index
    // into stock
    std::strncpy(stock, tickers[index], 8);

    // Set last byte to null terminator string
    stock[7] = '\0';
}