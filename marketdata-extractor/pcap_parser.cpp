#include "headers/pcap_parser.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>



PcapParser::PcapParser(const std::string& filename) : infile(filename, std::ios::binary), globalHeader{} {}

bool PcapParser::hasMorePackets() {
	return infile.peek() != EOF;
}

void PcapParser::parseGlobalHeader()
{
	if (!infile) {
		throw std::runtime_error("Error: Could not open pcap file.");
	}

	infile.read(reinterpret_cast<char*>(&globalHeader), sizeof(PcapGlobalHeader));

	if (!infile) {
		throw std::runtime_error("Error: Reading global header.");
	}

}

std::vector<uint8_t> PcapParser::parseNextPacket() {

	std::vector<uint8_t> result;

	if (infile.peek() == EOF) return result;

	PcapPacketHeader packetHeader;
	if (!infile.read(reinterpret_cast<char*>(&packetHeader), sizeof(PcapPacketHeader)))
		throw std::runtime_error("Error: reading PcapPacketHeader.");

	result.resize(packetHeader.caplen);

	if (!infile.read(reinterpret_cast<char*>(result.data()), packetHeader.caplen))
		throw std::runtime_error("Error: reading packet data.");

	if (packetHeader.caplen >= sizeof(EthernetHeader)) {
		EthernetHeader* eth = reinterpret_cast<EthernetHeader*>(result.data());
		uint16_t eth_type = swap16(eth->eth_type);

		if (eth_type == 0x0800 && packetHeader.caplen >= sizeof(EthernetHeader) + sizeof(IPv4Header)) {
			IPv4Header* ip = reinterpret_cast<IPv4Header*>(result.data() + sizeof(EthernetHeader));

			size_t ip_header_length = (ip->ihl_version & 0x0F) * 4;
			if (ip->protocol == 6 && packetHeader.caplen >= sizeof(EthernetHeader) + ip_header_length + sizeof(TCPHeader)) {
				TCPHeader* tcp = reinterpret_cast<TCPHeader*>(result.data() + sizeof(EthernetHeader) + ip_header_length);
				result = std::vector<uint8_t>(result.begin() + sizeof(EthernetHeader) + ip_header_length + sizeof(TCPHeader), result.end());
			}
			else if (ip->protocol == 17 && packetHeader.caplen >= sizeof(EthernetHeader) + ip_header_length + sizeof(UDPHeader)) {
				UDPHeader* udp = reinterpret_cast<UDPHeader*>(result.data() + sizeof(EthernetHeader) + ip_header_length);
				result = std::vector<uint8_t>(result.begin() + sizeof(EthernetHeader) + ip_header_length + sizeof(UDPHeader), result.end());
			}
		}
	}
	return result;
}