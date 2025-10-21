#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <constants.hpp>

int main()
{
    // Create a UDP socket for sending byte stream
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
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
    dest_addr.sin_port = htons(PORT); // Convert host architecture storage to big endian (network order)
    int rcode = inet_pton(AF_INET, MULTICAST_IP, &dest_addr.sin_addr);

    if (rcode == 0) {
        perror("Binary form of IPv4 multicast address %s not converted properly.\n");
        return 1;
    } else if (rcode == -1) {
        perror("Failed to create binary form of IPv4 multicast address.\n");
    }


    return 0;
}