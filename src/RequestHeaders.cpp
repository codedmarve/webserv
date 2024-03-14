#include "../inc/HttpRequestParser.hpp"

int HttpRequestParser::parseHeaders(const std::string &headerLines) {
    std::istringstream iss(headerLines);
    std::string line;
    std::string currentHeader;
    while (std::getline(iss, line, '\n')) {
        line = trim(line);

        if (line.empty()) // Empty line indicates end of headers
            break;

        if (line[0] == ' ' || line[0] == '\t') {
            
            if (!currentHeader.empty()) { // Multi-line header continuation
                headers_[currentHeader] += " " + line; // Append to the previous header value
            }
        } else {
            // New header line
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                headers_[key] = value;
                currentHeader = key; // Save current header for multi-line continuation
            } else {
                    // Invalid header format, log error or skip
                    std::cerr << "Invalid header format: " << line << std::endl;
                    // Optionally, we can throw an exception here
                    // throw std::runtime_error("Invalid header format");
                return 400;
            }
        }
    }
    return (200);
}

