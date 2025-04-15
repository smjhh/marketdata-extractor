#pragma once

#include <cstdint>
#include <vector>
#include <fstream>
#include <string>


#pragma pack(push, 1)

// Structure for PCAP Global Header (24 bytes)
struct PcapGlobalHeader {
	uint32_t magic_number;
	uint16_t version_major;
	uint16_t version_minor;
	int32_t thiszone;
	uint32_t sigfigs;
	uint32_t snaplen;
	uint32_t network;
};


// Structure for Packet Header (16 bytes)
struct PcapPacketHeader {
	uint32_t ts_sec;
	uint32_t ts_usec;
	uint32_t caplen;
	uint32_t len;
};


// Ethernet Header (14 bytes)
struct EthernetHeader {
	uint8_t dest_mac[6];
	uint8_t src_mac[6];
	uint16_t eth_type;
};

// IPv4 Header (min 20 bytes)
struct IPv4Header {
	uint8_t ihl_version;
	uint8_t tos;
	uint16_t total_length;
	uint16_t identification;
	uint16_t flags_frag_offset;
	uint8_t ttl;
	uint8_t protocol;
	uint16_t checksum;
	uint32_t src_ip;
	uint32_t dest_ip;
};

// TCP Header (min 20 bytes)
struct TCPHeader {
	uint16_t src_port;
	uint16_t dest_port;
	uint32_t seq_number;
	uint32_t ack_number;
	uint8_t data_offset_reserved;
	uint8_t flags;
	uint16_t window_size;
	uint16_t checksum;
	uint16_t urgent_pointer;
};

// UDP Header (8 bytes)
struct UDPHeader {
	uint16_t src_port;
	uint16_t dest_port;
	uint16_t length;
	uint16_t checksum;
};

#pragma pack(pop)

// Convert 16-bit (short) from network to host byte order
inline uint16_t swap16(uint16_t val) {
	return (val >> 8) | (val << 8);
}

// Convert 32-bit (int) from network to host byte order
inline uint32_t swap32(uint32_t val) {
	return ((val >> 24) & 0x000000FF) |
		((val >> 8) & 0x0000FF00) |
		((val << 8) & 0x00FF0000) |
		((val << 24) & 0xFF000000);
}

// Convert IP address from uint32_t to std::string
inline std::string ipTostdstring(uint32_t ip) {
	return std::to_string((ip >> 24) & 0xFF) + "." +
		std::to_string((ip >> 16) & 0xFF) + "." +
		std::to_string((ip >> 8) & 0xFF) + "." +
		std::to_string(ip & 0xFF);
}

class PcapParser {
public:
	explicit PcapParser(const std::string& filename);

	void parseGlobalHeader();
	bool hasMorePackets();
	std::vector<uint8_t> parseNextPacket();

private:
	std::ifstream infile;
	PcapGlobalHeader globalHeader;
};