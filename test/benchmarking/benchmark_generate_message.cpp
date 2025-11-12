#include <iostream>
#include <chrono>
#include "../../src/generator.h"
#include "../../src/messages.h"

int main() {
    std::cout << "=== ITCH Generator Benchmark ===\n";

    constexpr size_t TOTAL_MESSAGES = 1 << 24;
    constexpr size_t BUF_SIZE = 1 << 20; // 1 MB buffer
    alignas(64) uint8_t buf[BUF_SIZE];
    RetryBuffer retry{};
    size_t bytesRemaining = BUF_SIZE;

    // SIMULATE SENDING 2^24 NUMBER OF MESSAGES
    printf("Running benchmark for %zu messages...", TOTAL_MESSAGES);
    fflush(stdout);
    size_t totalBytesWritten = 0;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < TOTAL_MESSAGES; ++i) {
        ssize_t len = generateMessage(buf, retry, bytesRemaining);

        if (len > 0) {
            totalBytesWritten += len;
            bytesRemaining -= len;
        } else if (retry.size > 0) {
            // Flush retry buffer (simulate send)
            totalBytesWritten += retry.size;
            retry.size = 0;
            bytesRemaining = BUF_SIZE; // reset buffer
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Generated " << TOTAL_MESSAGES << " messages\n";
    std::cout << "Total bytes written: " << totalBytesWritten << "\n";
    std::cout << "Elapsed time: " << elapsed.count() << " seconds\n";
    std::cout << "Throughput: " 
              << (TOTAL_MESSAGES / elapsed.count()) << " messages/sec\n";
    std::cout << "Average message size: " 
              << (double)totalBytesWritten / TOTAL_MESSAGES << " bytes\n";

    return 0;
}
