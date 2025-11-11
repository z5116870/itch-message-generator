#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

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
        return 1;
    }

    std::cout << "Setup socket and dest addr succesfully.\n";

    LOGSERVER("STARTING GENERATOR");

    // Keep these buffers cache aligned 
    // Send message buffer (we wont use the entire space, but prevents overflowing)
    alignas(64) uint8_t sendBuf[3000];

    // Retry buffer
    alignas(64) retryBuffer retryBuf;

    // Position counter
    ssize_t pos = 0;

    // Send data to socket 
    while(1) {
        // Write directly to user-space send buffer (sendBuf)
        // only if writing would not exceed the SEND_BUFFER_SIZE
        ssize_t len = generateMessage(sendBuf + pos, retryBuf, SEND_BUFFER_SIZE - pos);
        pos += len; // update current pos

        // If sendBuf capacity exceeded, the retry buffer (buffer that contains the generatedMessage that was not
        // able to fit in the sendBuf) would have been written to in generateMessage. So just flush the sendBuf and 
        // then copy the contens of the retryBuf into the sendBuf, that triggers the flush, and this must be the first message written
        if (!len) {
            // Flush the buffer
            ssize_t bytesToSend = pos - len;
            ssize_t sent = sendto(sockfd, sendBuf, bytesToSend, 0, (sockaddr*) &dest_addr, sizeof(dest_addr));
            if(sent < 0) {
                perror("Could not send data");
            } else {
                std::cout << "Sent bytes: " << bytesToSend << std::endl << std::endl;
            }

            // Copy in retry buffer contents before next generateMessage
            memcpy(sendBuf, retryBuf.buf, retryBuf.size);
            pos = len;
            std::cin.get();
        }

        fflush(stdout);
    }
    return 0;
}