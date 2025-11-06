#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "messages.h"
#include "generator.h"

#define MULTICAST_IP "239.1.1.1"
#define PORT 30001
#define MAX_ITCH_MSG_SIZE 64
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
    uint8_t buf[MAX_ITCH_MSG_SIZE];

    // Send data to socket
    while(1) {
        ssize_t len = generateMessage(buf);
        ssize_t sent = sendto(sockfd, &buf, len, 0, (sockaddr*) &dest_addr, sizeof(dest_addr));

        if(sent < 0) {
            perror("Could not send data");
        } else {
            std::cout << "Sent bytes: " << len << std::endl << std::endl;
        }
        sleep(1);
        fflush(stdout);
    }
    return 0;
}