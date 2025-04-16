High-Performance PCAP Exchange Message Decoder
A high-performance C++ parser for decoding PCAP files containing exchange messages. This application extracts both incremental and snapshot market data based on the provided exchange documentation and message schema. The output is exported in JSON format for streamlined analysis and storage.
________________________________________
Overview
This project implements a custom parser that:
•	Reads and decodes PCAP files.
•	Extracts raw market data messages in both incremental and snapshot formats.
•	Converts and exports the decoded messages to JSON for further analysis.
•	Is optimized for speed and performance, handling complex data formats directly in C++.
The parser leverages standard C++ libraries along with careful byte-level manipulations to ensure that the data is parsed correctly from low-level networking headers, through IP/TCP/UDP layers, to the exchange-specific market data.
________________________________________
Features
•	High-Performance Parsing: Designed in C++ for optimal speed and minimal overhead when processing large PCAP files.
•	Multi-Layer Data Extraction: Handles decoding of Ethernet, IP, TCP, and UDP headers to extract the embedded market data.
•	Incremental & Snapshot Data: Supports both incremental updates and full snapshots of the order book.
•	JSON Export: Outputs the extracted market data in JSON format, making it easy to store, analyze, or integrate with other systems.
•	Robust Error Handling: Incorporates thorough checks to ensure that incomplete or corrupt packets are managed appropriately.
________________________________________
Code Structure
•	Main Application (main.cpp):
Orchestrates the parsing process. It takes an input PCAP file and an output JSON file as command-line arguments and then decodes each packet in sequence.
•	PcapParser:
o	Reads the PCAP file.
o	Parses the global header and packet headers.
o	Extracts the payload by skipping over Ethernet, IP, TCP/UDP layers.
•	SimbaDecoder:
o	Decodes market data messages from the extracted packet data.
o	Converts message fields such as timestamps, flags, and order data into JSON strings.
o	Handles both incremental packet headers and market messages.
•	JsonWriter:
o	Manages the creation of a JSON array to which each decoded message is appended.
o	Ensures the JSON output is well-formed and ready for analysis or further processing.
•	Header Files:
Provides necessary declarations for data structures like PcapGlobalHeader, MarketDataPacketHeader, and various exchange message types (Order Update, Order Execution, Order Book Snapshot).
________________________________________
Build Instructions
Prerequisites
•	A C++ compiler that supports C++11 (or later).
•	A standard build system (e.g., Make, CMake) if you prefer to use build scripts.
•	Basic understanding of compiling C++ projects from the command line.
Compilation
If you prefer to build from the command line, you can compile the project with a command similar to:
bash
CopyEdit
g++ -std=c++11 -O2 -o pcap_decoder main.cpp headers/pcap_parser.cpp headers/decoder.cpp headers/json_writer.cpp
Make sure to replace the file paths with the correct ones if your project’s structure differs.
Using CMake
You may create a simple CMakeLists.txt file:
cmake
CopyEdit
cmake_minimum_required(VERSION 3.5)
project(PCAPDecoder)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O2")

include_directories(headers)

add_executable(pcap_decoder main.cpp pcap_parser.cpp decoder.cpp json_writer.cpp)
Then compile the project:
bash
CopyEdit
mkdir build && cd build
cmake ..
make
________________________________________
Usage
After compiling, run the executable by providing a PCAP file as input and specifying the output JSON file. For example:
bash
CopyEdit
./pcap_decoder sample_input.pcap output.json
The program reads the PCAP file, decodes each packet, converts the market data into JSON, and writes the output to the given JSON file.
________________________________________
Demo
Check out the Demo Video (replace # with your demo link) for a walkthrough of the decoding process and how the JSON output is generated from sample PCAP files.
________________________________________
Contributing
Contributions to improve performance, add new features, or address bugs are welcome. Please fork the repository and open a pull request with your proposed changes.
1.	Fork the repository.
2.	Create a feature branch: git checkout -b feature/new-feature
3.	Commit your changes: git commit -am 'Add new feature'
4.	Push to the branch: git push origin feature/new-feature
5.	Create a new Pull Request.
________________________________________
License
This project is licensed under the MIT License - see the LICENSE file for details.
________________________________________
Acknowledgements
•	Thanks to the exchange documentation and schema that provided the basis for this implementation.
•	Inspired by the need for rapid market data analysis and efficient data handling in high-performance computing environments.

