# itch-message-generator

**itch-message-generator** is a C++ application that generates ITCH protocol messages and sends them via UDP to a multicast IP address.  
It’s designed for local testing or simulation of market-data–style traffic, allowing developers to generate structured “ITCH” type messages and transmit them over a network.

---

## Table of Contents
- [Background](#background)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
  - [Running](#running)
- [Usage](#usage)
  - [Configuration](#configuration)
  - [Message Generation](#message-generation)
  - [UDP / Multicast Settings](#udp--multicast-settings)
- [Structure & Code Overview](#structure--code-overview)
- [Dependencies](#dependencies)

---

## Background

The **ITCH protocol** – used by stock exchanges such as NASDAQ – is a binary message format transmitting order-book updates, trades, cancellations, and other market events.  

This tool simulates such message flows locally: it generates a stream of ITCH messages and sends them over multicast/UDP for downstream consumers such as order-book processors or latency test systems.

---

## Features

- Generate a variety of ITCH-style messages (orders, executions, etc.)
- Send messages via UDP multicast
- Configurable message rate, type, and payload contents
- Minimal external dependencies (pure standard C++)
- Example code for serialization and network sending

---

## Getting Started

### Prerequisites

- A C++ compiler supporting at least C++11 (e.g., GCC, Clang, or MSVC)
- A system with multicast support
- (Optional) CMake or other build system

### Building

Example command-line build:

```bash
git clone https://github.com/z5116870/itch-message-generator.git
cd itch-message-generator
g++ -std=c++11 main.cpp messages.cpp helper.cpp -o itch_gen
```
If you use CMake, create a minimal CMakeLists.txt including the above sources.

### Running
bash
Copy code
./itch_gen <multicast_address> <port>
Example:

bash
Copy code
./itch_gen 239.0.0.1 5000
The program will start generating ITCH messages and broadcasting them to the provided multicast address and port.

## Usage
Configuration
Modify parameters in the source (e.g., helper.h, messages.h) to set:

Multicast IP and port

Message types and generation rates

Payload structure (IDs, sizes, prices, etc.)

### Message Generation
messages.cpp and messages.h define the ITCH message formats and serialization logic.
Each message corresponds to a protocol type, with fields like order ID, timestamp, and price.
main.cpp orchestrates message creation and sending.

### UDP / Multicast Settings
Ensure your OS/network allows sending to multicast groups

Use a receiver listening on the same multicast IP/port

Adjust TTL or loopback options as needed

## Structure & Code Overview
bash
Copy code
itch-message-generator/
├── main.cpp         # Entry point: sets up generator, network socket, loop
├── messages.h/.cpp  # ITCH message type definitions and serialization
├── helper.h/.cpp    # Utility functions (networking, random data, timestamps)
└── README.md        # Project documentation

File Details
main.cpp – Initializes multicast UDP socket, runs message-generation loop.

messages.h / .cpp – Defines message structures and encoding logic.

helper.h / .cpp – Provides networking utilities and helper functions.

## Dependencies
Standard C++ library

BSD sockets (Linux/macOS) or Winsock (Windows)

No external libraries required

