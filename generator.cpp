#include "generator.h"

size_t generateMessage(uint8_t *buf, retryBuffer *retryBuf, const size_t &bytesRemaining) {
    // Generator random index
    std::uniform_int_distribution<uint32_t> dist(0,NUM_MESSAGES-1);
    uint32_t chosenGenerator = dist(gen);

    // Call serialize function on random index, return size written
    size_t msgLen = generators[chosenGenerator](buf);
    if (msgLen > bytesRemaining) {
        // Copy the last written message
        memcpy(retryBuf->buf, buf, msgLen);
        retryBuf->size = msgLen;
        retryBuf->valid = true;
        std::cout << "retry write\n";
    }
    std::cout << "bytes remaining: " << bytesRemaining << std::endl;
    std::cout << "normal write\n";
    return msgLen;
}