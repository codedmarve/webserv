#include "../inc/HttpRequestParser.hpp"

// int HttpRequestParser::parseHeaders(const std::string &headerLines) {
//     std::istringstream iss(headerLines);
//     std::string line;
//     std::string currentHeader;
//     while (std::getline(iss, line, '\n')) {
//         line = trim(line);

//         if (line.empty()) // Empty line indicates end of headers
//             break;

//         if (line[0] == ' ' || line[0] == '\t') {
            
//             if (!currentHeader.empty()) { // Multi-line header continuation
//                 headers_[currentHeader] += " " + line; // Append to the previous header value
//             }
//         } else {
//             // New header line
//             size_t pos = line.find(":");
//             if (pos != std::string::npos) {
//                 std::string key = trim(line.substr(0, pos));
//                 std::string value = trim(line.substr(pos + 1));
//                 headers_[key] = value;
//                 currentHeader = key; // Save current header for multi-line continuation
//             } else {
//                     // Invalid header format, log error or skip
//                     std::cerr << "Invalid header format: " << line << std::endl;
//                     // Optionally, we can throw an exception here
//                     // throw std::runtime_error("Invalid header format");
//                 return 400;
//             }
//         }
//     }
//     return (200);
// }


int HttpRequestParser::parseHeaders(const std::string& headerLines) {
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

                // Check for "Host" header
                if (key == "Host" && value.empty()) {
                    std::cerr << "Empty 'Host' header." << std::endl;
                    // [important] validate Host header here
                    return 400; // Bad Request: Empty Host header
                }

                // Check for "Transfer-Encoding" header
                if (key == "Transfer-Encoding" && value == "chunked") {
                    isChunked_ = true;
                }

                // Check for "Content-Length" header
                if (key == "Content-Length") {
                    // Check if the value is a valid integer
                    if (value.find_first_not_of("0123456789") != std::string::npos) {
                        std::cerr << "Invalid 'Content-Length' header value." << std::endl;
                        return 400; // Bad Request: Invalid Content-Length header value
                    }

                    // Convert the value to an integer
                    try {
                        length_ = std::stoi(value);
                        if (length_ < 0) {
                            throw std::invalid_argument("Negative content-length");
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing 'Content-Length' header: " << e.what() << std::endl;
                        return 400; // Bad Request: Error parsing Content-Length header
                    }
                } 
            }
        }
    }
    return 200; // OK: Headers parsed successfully
}
