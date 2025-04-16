# High-Performance PCAP Exchange Message Decoder

A high-performance C++ parser for decoding PCAP files containing exchange messages. This application extracts both incremental and snapshot market data based on the provided exchange documentation and message schema. The output is exported in JSON format for streamlined analysis and storage.

## Overview

This project implements a custom parser that:

- **Reads and decodes PCAP files.**
- **Extracts raw market data messages** in both incremental and snapshot formats.
- **Converts and exports the decoded messages to JSON**, making further analysis and storage easier.
- **Delivers high performance:** Optimized in C++ for speed and efficiency in processing large PCAP files.

The parser performs meticulous byte-level manipulations to accurately process data across multiple network layers—from Ethernet/IP/TCP/UDP headers to the exchange-specific market messages.

## Features

- **High-Performance Parsing:** Developed in C++ to achieve optimal speed and minimal overhead for large datasets.
- **Multi-Layer Extraction:** Handles decoding of Ethernet, IP, TCP, and UDP headers to access embedded market data.
- **Incremental & Snapshot Data Support:** Supports both real-time incremental updates and full order book snapshots.
- **JSON Output:** The extracted data is formatted as JSON for easy downstream integration with various analysis tools.
- **Robust Error Handling:** Designed to manage incomplete or corrupt packets gracefully.

## Code Structure

- **`main.cpp`:**  
  Orchestrates the decoding process by accepting an input PCAP file and an output JSON file from the command-line. It then sequentially decodes each packet.
  
- **PcapParser:**  
  - Reads the PCAP file.
  - Parses the global and packet headers.
  - Extracts the payload by removing Ethernet, IP, TCP/UDP layers.
  
- **Decoder:**  
  - Decodes market data messages from the extracted packet data.
  - Handles both incremental packet headers and a variety of market messages.
  - Outputs JSON representations of the decoded data.
  
- **JsonWriter:**  
  - Writes the decoded messages into a well-formed JSON array.
  - Appends each message as it is decoded, ensuring proper formatting for analysis.
  
- **Header Files:**  
  Define critical data structures such as `PcapGlobalHeader`, `MarketDataPacketHeader`, and various market data messages including Order Update, Order Execution, and Order Book Snapshot.

## Build Instructions

### Prerequisites

- A C++ compiler that supports C++11 or later.
- A standard build system such as Make or CMake.
- Basic knowledge of compiling C++ projects from the command line.

### Compiling from the Command Line

You can compile the project using a command similar to:

```bash
g++ -std=c++11 -O2 -o pcap_decoder main.cpp pcap_parser.cpp decoder.cpp json_writer.cpp
```

## Using CMake

If you prefer to use CMake, create a `CMakeLists.txt` file with the following content:

```cmake
cmake_minimum_required(VERSION 3.5)
project(PCAPDecoder)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2")

include_directories(headers)

add_executable(pcap_decoder main.cpp pcap_parser.cpp decoder.cpp json_writer.cpp)
```

Then compile with:

```bash
mkdir build && cd build
cmake ..
make
```

### Usage
Once compiled, run the executable with the following command:
```bash
./pcap_decoder sample_input.pcap output.json
```
- **`sample_input.pcap`:** The PCAP file containing exchange messages.
- **`output.json`:** The file where decoded JSON data is saved.

The program decodes the PCAP file, converts market data messages to JSON, and writes the output for analysis or further processing.

### Demo
Check out the Demo Video for a walkthrough of the decoding process and the JSON output
[![Watch the video](https://img.youtube.com/vi/03rHsjZ09R4/0.jpg)](https://youtu.be/03rHsjZ09R4)


### License
This project is licensed under the MIT License.

### Acknowledgements
- Thanks to the exchange documentation and message schema that informed this implementation.
- Inspired by the need for rapid market data analysis in high-performance computing environments.
