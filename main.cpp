#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "messages.h"
#include "generator.h"

// CONSTANTS
#define MULTICAST_IP "239.1.1.1"
#define PORT 30001
constexpr ssize_t SEND_BUFFER_SIZE = 1472;

// MACROS
#define LOG(x) std::cout << x << std::endl
#define SEPARATOR "----------------------"
#define LOGSERVER(x) LOG(SEPARATOR); LOG(x); LOG(SEPARATOR)

int main()
{
    LOGSERVER("STARTING ITCH MESSAGE GENERATOR...");
    // Create a UDP socket for sending byte stream
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0) {
        perror("Failed to initialize a socket.\n");
        return 1;
    }

    LOGSERVER("INITIALISED SOCKET");
    // Set socket options, set to process IPv4 traffic (IPPROTO_IP) with TTL value to 1
    int ttl = 1; // We dont want the packet to be forwarded more than once (outside the local network)

    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0)
    {
        perror("Failed to set options on UDP socket.\n");
        return 1;
    }

    LOGSERVER("SET SOCKET OPTIONS SUCCESSFULLY");
    // Setup multicast IP address for sending data
    sockaddr_in dest_addr{};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT); // Convert host (little/big endian) to big endian (network order)
    int rcode = inet_pton(AF_INET, MULTICAST_IP, &dest_addr.sin_addr);

    if (rcode == 0) {
        fprintf(stderr, "Binary form of IPv4 multicast address %s not converted properly.\n", MULTICAST_IP);
        return 1;
    } else if (rcode == -1) {
        perror("Failed to create binary form of IPv4 multicast address.\n");
    }

    std::cout << "Setup socket and dest addr succesfully.\n";

    LOGSERVER("STARTING GENERATOR");

    // Send message buffer and current position pointer;
    uint8_t sendBuf[SEND_BUFFER_SIZE];
    retryBuffer retryBuf;
    ssize_t pos = 0;

    char addrbuf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &dest_addr.sin_addr, addrbuf, sizeof(addrbuf));
    std::cout << "Sending to " << addrbuf 
          << ":" << ntohs(dest_addr.sin_port)
          << ", family=" << dest_addr.sin_family << std::endl;
    // Send data to socket 
    while(1) {
        // Write directly to user-space send buffer (sendBuf)
        // only if writing would not exceed the SEND_BUFFER_SIZE
        // We also have a retry buffer that stores a copy of the message
        // that triggers the flush, and this must be the first message written
        // if it exists
        if (retryBuf.valid) {
            pos = 0;
            memcpy(sendBuf, retryBuf.buf, retryBuf.size);
            pos += retryBuf.size;
            retryBuf.valid = 0;
            continue;
        }

        // If the retry buffer is not valid, then continue with storage in normal send
        // buffer (sendBuf)
        ssize_t len = generateMessage(sendBuf + pos, &retryBuf, SEND_BUFFER_SIZE - pos);
        ssize_t sent = sendto(sockfd, sendBuf, len, 0, (sockaddr*) &dest_addr, sizeof(dest_addr));
        pos += len; // update current pos

        if(sent < 0) {
            perror("Could not send data");
        } else {
            std::cout << "Sent bytes: " << len << std::endl << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        fflush(stdout);
    }
    return 0;
}