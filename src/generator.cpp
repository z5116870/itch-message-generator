#include "generator.h"

ssize_t generateMessage(uint8_t *buf, RetryBuffer &retryBuf, const size_t &bytesRemaining) {
    // Generator random index
    std::uniform_int_distribution<uint32_t> dist(0,NUM_MESSAGES-1);
    uint32_t chosenGenerator = dist(gen);

    // Call serialize function on random index, return size written
    size_t msgLen = generators[chosenGenerator](buf);
    if (msgLen > bytesRemaining) {
        // Copy the last written message
        memcpy(retryBuf.buf, buf, msgLen);
        retryBuf.size = msgLen;
        return 0;
    }

    // std::cout << "bytes remaining: " << bytesRemaining << std::endl;
    // std::cout << "normal write\n";
    return msgLen;
}