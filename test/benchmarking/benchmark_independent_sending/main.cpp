#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include "messages.h"
#include "generator.h"

#define MULTICAST_IP "239.1.1.1"
#define PORT 30001
#define MAX_ITCH_MSG_SIZE 64
#define LOG(x) std::cout << x << std::endl
#define SEPARATOR "----------------------"
#define LOGSERVER(x) \
    LOG(SEPARATOR);  \
    LOG(x);          \
    LOG(SEPARATOR)
constexpr uint32_t ITCH_MESSAGES_TO_SEND = 1000000;
int main()
{
    // Create a UDP socket for sending byte stream
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
    {
        perror("Failed to initialize a socket.\n");
        return 1;
    }
    // Set socket options, set to process IPv4 traffic (IPPROTO_IP) with TTL value to 1
    int ttl = 1; // We dont want the packet to be forwarded more than once (outside the local network)

    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("Failed to set options on UDP socket.\n");
        return 1;
    }
    // Setup multicast IP address for sending data
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT); // Convert host (little/big endian) to big endian (network order)
    dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int sendBufSize = 4 * 1024 * 1024;
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, sizeof(sendBufSize)) < 0) {
        perror("Failed to set send buffer size");
    }
    
    uint8_t buf[3000];
    std::cout << "=== ITCH Generator Benchmark (Independent Sending)===\n";
    printf("Running benchmark for sending %u batches ITCH messages...\n\n", ITCH_MESSAGES_TO_SEND);
    
    auto now = std::chrono::steady_clock::now();

    // MEASURES TIME TAKEN TO MAKE SENDTO() SYSCALL FOR 100 ITCH MESSAGES
    while (GlobalMessageState::sequenceNumber < ITCH_MESSAGES_TO_SEND)
    {
        ssize_t len = generateMessage(buf);
        ssize_t sent = sendto(sockfd, buf, len, 0, (sockaddr *)&dest_addr, sizeof(dest_addr));

        if (sent < 0)
        {
            perror("Could not send data");
            continue;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    auto end = std::chrono::steady_clock::now();
    auto time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - now).count();
    std::chrono::duration<double> time_taken_sec = end - now;
    std::cout << "=== RESULTS ===\n";
    std::cout << "ITCH messages sent: " << GlobalMessageState::sequenceNumber << std::endl;
    std::cout << "Total time taken: " << time_taken << " ns" << std::endl;
    std::cout << "Average time taken (per message sent): " << (time_taken / ITCH_MESSAGES_TO_SEND) << " ns" << std::endl;
    std::cout << "Throughput " << GlobalMessageState::sequenceNumber / time_taken_sec.count() << " messages/sec\n";
    return 0;
}