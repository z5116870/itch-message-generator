#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>

#include "../../src/messages.h"
#include "../../src/generator.h"

#define MULTICAST_IP "239.1.1.1"
#define PORT 30001
constexpr ssize_t SEND_BUFFER_SIZE = 1472;
constexpr uint32_t ITCH_MESSAGES_TO_SEND = 1000000;

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Failed to initialize a socket.\n");
        return 1;
    }
    int ttl = 1;

    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("Failed to set options on UDP socket.\n");
        return 1;
    }
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    int rcode = inet_pton(AF_INET, MULTICAST_IP, &dest_addr.sin_addr);

    if (rcode == 0) {
        fprintf(stderr, "Binary form of IPv4 multicast address %s not converted properly.\n", MULTICAST_IP);
        return 1;
    } else if (rcode == -1) {
        perror("Failed to create binary form of IPv4 multicast address.\n");
        return 1;
    }
    std::cout << "=== ITCH Generator Benchmark (Batched Sending)===\n";

    printf("Running benchmark for %u ITCH messages sent...\n\n", ITCH_MESSAGES_TO_SEND);
    fflush(stdout);
    alignas(64) uint8_t sendBuf[3000];
    alignas(64) RetryBuffer retryBuf{};
    ssize_t pos = 0;

    auto start = std::chrono::steady_clock::now();
    // Send data to socket 
    while (GlobalMessageState::sequenceNumber < ITCH_MESSAGES_TO_SEND) {
        // Write directly to user-space send buffer (sendBuf)
        // only if writing would not exceed the SEND_BUFFER_SIZE
        ssize_t len = generateMessage(sendBuf + pos, retryBuf, SEND_BUFFER_SIZE - pos);
        pos += len; // update current pos

        // If sendBuf capacity exceeded, the retry buffer (buffer that contains the generatedMessage that was not
        // able to fit in the sendBuf) would have been written to in generateMessage. So just flush the sendBuf and 
        // then copy the contens of the retryBuf into the sendBuf, that triggers the flush, and this must be the first message written
        if (!len) {
            // Flush the buffer
            ssize_t bytesToSend = pos;
            ssize_t sent = sendto(sockfd, sendBuf, bytesToSend, 0, (sockaddr*) &dest_addr, sizeof(dest_addr));
            if(sent < 0) {
                perror("Could not send data");
            }

            // Copy in retry buffer contents before next generateMessage
            memcpy(sendBuf, retryBuf.buf, retryBuf.size);
            pos = retryBuf.size;
        }

    }

    // Flush remaining bytes (in case the last batch isn't full)
    if (pos > 0) {
        sendto(sockfd, sendBuf, pos, 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
    }

    auto end = std::chrono::steady_clock::now();
    long long totalTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "=== RESULTS ===\n";
    printf("ITCH messages sent: %u\n", GlobalMessageState::sequenceNumber);
    printf("Total time taken: %llu ns\n", totalTimeNs);
    printf("Average time taken (per message sent): %llu ns\n", totalTimeNs/ITCH_MESSAGES_TO_SEND);
    return 0;
}