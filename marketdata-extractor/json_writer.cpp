#include "headers/json_writer.h"
#include <sstream>

JsonWriter::JsonWriter(const std::string& filename) : out(filename), firstMessage(true) {
	if (!out) {
		throw std::runtime_error("Error opening output file for JSON.");
	}
	out << "[\n";  // Start the JSON array
}

JsonWriter::~JsonWriter() {
	out << "\n]";  // End the JSON array
	out.close();
}

void JsonWriter::appendMessageToFile(const std::ostringstream& message) {
	if (!out) {
		throw std::runtime_error("Error: Output file stream is not open.");
	}

	const std::string& msgStr = message.str();
	if (msgStr.empty()) {
		throw std::runtime_error("Warning: Attempted to write an empty message.");
	}

	if (firstMessage) {
		out << msgStr;  // Write the first message without a comma
		firstMessage = false;  // Subsequent messages will have a comma
	}
	else {
		out << ",\n" << msgStr;  // Write a comma before the message
	}
}