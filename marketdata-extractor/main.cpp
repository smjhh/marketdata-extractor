
#include "headers/pcap_parser.h"
#include "headers/decoder.h"
#include "headers/json_writer.h"




int main(int argc, char* argv[]) {

	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " input.pcap output.json" << std::endl;
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];


	try {
		std::cout << "Decoding Started..." << std::endl;
		PcapParser parser(inputFile);
		parser.parseGlobalHeader();

		JsonWriter writer(outputFile);

		while (parser.hasMorePackets()) {
			auto packet = parser.parseNextPacket();
			if (!packet.empty()) {
				Decoder decoder(packet);
				decoder.decode();
				decoder.toJSON();
				writer.appendMessageToFile(decoder.getJSON());
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	std::cout << "Decoding complete. Output saved to " << outputFile << std::endl;
	return 0;
}

