#include "generator.h"

size_t generateMessage(uint8_t *buf) {
    // Generator random index
    std::uniform_int_distribution<uint32_t> dist(0,NUM_MESSAGES-1);
    uint32_t chosenGenerator = dist(gen);

    // Call serialize function on random index, return size written
    return generators[chosenGenerator](buf);
}