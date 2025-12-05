#include <gtest/gtest.h>
#include <cstdint>
#include "generator.h"
#include "messages.h"
#include "src/helper.h"

// ------------------------------------------------------------
// Tests for generateMessage() functionality
// ------------------------------------------------------------

// Normal message generation within buffer limit
TEST(GeneratorTest, NormalWrite) {
    // GIVEN a buffer that is empty
    RetryBuffer retry{};
    alignas(64) uint8_t buf[2048];
    size_t bytesRemaining = 200;

    // WHEN we try to generate a message and store it in the buffer
    ssize_t len = generateMessage(buf, retry, bytesRemaining);

    // THEN we expect the bytes to get written, and the retry buffer should be
    // unchanged i.e. 0 size
    EXPECT_GT(len, 0) << "Expected nonzero message length";
    EXPECT_EQ(retry.size, 0) << "Retry buffer should remain empty on normal write";
}

// Retry buffer triggers when insufficient bytes remaining
TEST(GeneratorTest, RetryBufferTriggered) {
    // GIVEN a buffer that has a smaller capacity than all ITCH messages
    // i.e. almost full
    RetryBuffer retry{};
    alignas(64) uint8_t buf[128];
    size_t bytesRemaining = 4;

    // WHEN we attempt to generate a message and store it in the buffer
    ssize_t len = generateMessage(buf, retry, bytesRemaining);

    // THEN we expect no bytes to be written to buf, but instead to the retry buffer
    EXPECT_EQ(len, 0) << "Expected 0 return because retry triggered";
    EXPECT_GT(retry.size, 0) << "Retry buffer should have stored last message";
}

// Sequence number increments monotonically
TEST(GeneratorTest, SequenceNumberMonotonic) {
    // GIVEN the current sequence number
    RetryBuffer retry{};
    alignas(64) uint8_t buf[4096];
    uint32_t prevSeq = GlobalMessageState::sequenceNumber;

    // WHEN we generate 1000 messages
    for (int i = 0; i < 1000; ++i)
        generateMessage(buf, retry, sizeof(buf));

    uint32_t newSeq = GlobalMessageState::sequenceNumber;

    // THEN we expect the sequence number to have advanced by 1000
    EXPECT_EQ(newSeq, prevSeq + 1000) << "Sequence number did not increase correctly";
}

// Send buffer alignment is cache-friendly (64-byte)
TEST(GeneratorTest, BufferAlignment) {
    // GIVEN a ITCH message send buffer
    // WHEN we try to align it to 64 bytes (to prevent false sharing)
    alignas(64) uint8_t buf[1472];
    
    uintptr_t addr = reinterpret_cast<uintptr_t>(buf);

    // THEN we expect that its address is a multiple of 64
    // i.e. 64 byte aligned
    EXPECT_EQ(addr % 64, 0) << "Buffer not 64-byte aligned";
}

// getNanoSecondsSinceMidnight() is monotonically increasing
TEST(GeneratorTest, TimingConsistency) {
    auto t1 = getNanoSecondsSinceMidnight();
    auto t2 = getNanoSecondsSinceMidnight();

    // THEN timestamps should increase or stay the same
    EXPECT_LE(t1, t2) << "Timestamps not increasing";
}