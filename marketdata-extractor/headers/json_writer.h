#pragma once

#include <fstream>

// Writes a collection of decoded messages to a file, one JSON object per line.
class JsonWriter {
public:
	JsonWriter(const std::string& filename);
	// Writes the decoded messages to the specified output file.
	void appendMessageToFile(const std::ostringstream& message);
	~JsonWriter();
private:
	std::ofstream out;  // Output file stream
	bool firstMessage;
};
