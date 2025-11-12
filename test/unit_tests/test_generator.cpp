// Test generateMessage() functionality
#include <iostream>
#include <cassert>
#include <cstring>
#include <chrono>

#include "../../src/generator.h"
#include "../../src/messages.h"
#include "../../src/helper.h"

// Simple test logging macros
#define TEST_CASE(name)  std::cout << "\n[TEST]: " << name << " ... "
#define PASS()           std::cout << "**PASS**" << std::endl << std::endl;
#define FAIL(msg)        do { std::cerr << "!!FAIL!!: " << msg << std::endl; std::exit(1); } while(0)


void testGenerateMessageNormalWrite() {
    TEST_CASE("Normal message generation within buffer limit");

    // GIVEN a buffer that is empty
    RetryBuffer retry{};
    alignas(64) uint8_t buf[2048];
    size_t bytesRemaining = 200;

    // WHEN we try to generate a message and store it in the buffer
    ssize_t len = generateMessage(buf, retry, bytesRemaining);

    // THEN we expect the bytes to get written, and the retry buffer should be
    // unchanged i.e. 0 size
    if (len == 0)
        FAIL("Expected nonzero message length");
    if (retry.size != 0)
        FAIL("Retry buffer should remain empty on normal write");

    PASS();
}

void testGenerateMessageTriggersRetryBuffer() {
    TEST_CASE("Retry buffer triggers when insufficient bytes remaining");

    // GIVEN a buffer that has a smaller capacity than all ITCH messages
    // i.e. almost full
    RetryBuffer retry{};
    alignas(64) uint8_t buf[128];
    size_t bytesRemaining = 4;

    // WHEN we attempt to generate a message and store it in the buffer
    ssize_t len = generateMessage(buf, retry, bytesRemaining);

    // THEN we expect no bytes to be written to buf, but instead to the retry buffer
    if (len != 0)
        FAIL("Expected 0 return because retry triggered");
    if (retry.size == 0)
        FAIL("Retry buffer should have stored last message");

    PASS();
}

void testMultipleGenerateSequenceNumberMonotonic() {
    TEST_CASE("Sequence number increments monotonically");

    // GIVEN the current sequence number
    RetryBuffer retry{};
    alignas(64) uint8_t buf[4096];
    uint32_t prevSeq = GlobalMessageState::sequenceNumber;

    // WHEN we generate 1000 messages
    for (int i = 0; i < 1000; ++i)
        generateMessage(buf, retry, sizeof(buf));

    uint32_t newSeq = GlobalMessageState::sequenceNumber;

    // THEN we expect the sequence number to have advanced by 1000
    if (newSeq <= prevSeq || newSeq != prevSeq + 1000)
        FAIL("Sequence number did not increase");

    std::cout << "(seq advanced from " << prevSeq << " → " << newSeq << ")\n";
    PASS();
}

void testBufferAlignment() {
    TEST_CASE("Send buffer alignment is cache-friendly (64-byte)");

    // GIVEN a ITCH message send buffer
    // WHEN we try to align it to 64 bytes (to prevent false sharing)
    alignas(64) uint8_t buf[1472];
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(buf);

    // THEN we expect that its address is a multiple of 64
    // i.e. 64 byte aligned
    if (addr % 64 != 0)
        FAIL("Buffer not 64-byte aligned");

    PASS();
}

void testTimingConsistency() {
    TEST_CASE("getNanoSecondsSinceMidnight() is monotonically increasing");

    auto t1 = getNanoSecondsSinceMidnight();
    auto t2 = getNanoSecondsSinceMidnight();

    if (t2 < t1)
        FAIL("Timestamps not increasing");

    PASS();
}

int main() {
    std::cout << "=== ITCH MESSAGE GENERATOR: Generator Unit Tests ===\n";
    testGenerateMessageNormalWrite();
    testGenerateMessageTriggersRetryBuffer();
    testMultipleGenerateSequenceNumberMonotonic();
    testBufferAlignment();
    testTimingConsistency();
    std::cout << "\nAll tests completed successfully.\n";
}
