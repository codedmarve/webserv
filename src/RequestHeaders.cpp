#include "../inc/HttpRequestParser.hpp"

// void HttpRequestParser::parseHeaders(const std::string &headerLines) {
//     std::istringstream iss(headerLines);
//     std::string line;
//     while (std::getline(iss, line, '\r')) {
//         if (!line.empty()) {
//             size_t pos = line.find(": ");
//             if (pos != std::string::npos) {
//                 std::string key = line.substr(0, pos);
//                 std::string value = line.substr(pos + 2);
//                 headers_[key] = value;
//             }
//         }
//     }
// }

void HttpRequestParser::parseHeaders(const std::string &headerLines) {
    std::istringstream iss(headerLines);
    std::string line;
    std::string currentHeader;
    while (std::getline(iss, line, '\n')) {
        // Trim leading and trailing whitespace
        line = trim(line);

        if (line.empty()) {
            // Empty line indicates end of headers
            break;
        }

        if (line[0] == ' ' || line[0] == '\t') {
            // Multi-line header continuation
            if (!currentHeader.empty()) {
                // Append to the previous header value
                headers_[currentHeader] += " " + line;
            }
        } else {
            // New header line
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                headers_[key] = value;
                currentHeader = key; // Save current header for multi-line continuation
            }
        }
    }
}

