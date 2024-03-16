

// void HttpRequestParser::parseChunkedBody(const std::string& chunkedBody) {
//         std::istringstream iss(chunkedBody);
//         std::string line;
//         std::string decodedBody;

//         while (std::getline(iss, line)) {
//             // Convert chunk size from hex to decimal
//             unsigned int chunkSize = hexToDecimal(line);
            
//             // Check for valid CRLF after chunk size
//             if (iss.get() != '\r' || iss.get() != '\n') {
//                 throw std::runtime_error("Invalid CRLF after chunk size");
//             }

//             if (chunkSize == 0) {
//                 // Last chunk, end of body
//                 break;
//             }

//             std::string chunkData;
//             for (unsigned int i = 0; i < chunkSize; ++i) {
//                 char c;
//                 if (!(iss.get(c))) {
//                     throw std::runtime_error("Invalid chunked body format");
//                 }
//                 chunkData += c;
//             }

//             // Check for valid CRLF after chunk data
//             if (iss.get() != '\r' || iss.get() != '\n') {
//                 throw std::runtime_error("Invalid CRLF after chunk data");
//             }

//             // Append chunk data to decoded body
//             decodedBody += chunkData;
//         }

//         body_ = decodedBody;
//     }

// std::string HttpRequestParser::parseBody(const std::string& contentType) {
//         if (isChunked_) {
//             // Parse chunked body
//             parseChunkedBody(body_);
//         } else {
//             // Non-chunked body, no parsing needed
//             // body_ = body_;
//         }

//         // Additional validation or parsing logic based on content type
//         if (contentType.find("application/json") != std::string::npos) {
//             // JSON body
//             validateJson(body_);
//             return body_;
//         } else if (contentType.find("application/xml") != std::string::npos) {
//             // XML body
//             validateXml(body_);
//             return body_;
//         } else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
//             // Form data
//             validateFormData(body_);
//             return body_;
//         } else if (contentType.find("text/plain") != std::string::npos) {
//             // Plain text body
//             validatePlainText(body_);
//             return body_;
//         } else if (contentType.find("multipart/form-data") != std::string::npos) {
//             // Multipart data
//             // validateMultipartData(body_);
//             return body_;
//         } else if (contentType.find("application/octet-stream") != std::string::npos) {
//             // Binary data
//             validateBinaryData(body_);
//             return body_;
//         } else {
//             // Unsupported content type
//             throw std::runtime_error("Unsupported content type: " + contentType);
//         }
//     }




// bool HttpRequestParser::validateJson(const std::string& jsonString) {
//     // std::vector<char> stack; // Stack to track nested objects and arrays
//     // bool inString = false;    // Flag to track if currently inside a string
//     // bool inEscape = false;    // Flag to track if the previous character was an escape character

//     // for (char c : jsonString) {
//     //     if (inEscape) {
//     //         inEscape = false; // Skip next character after escape
//     //         continue;
//     //     }

//     //     if (c == '"') {
//     //         inString = !inString; // Toggle string flag
//     //     }

//     //     if (!inString) {
//     //         if (c == '{' || c == '[') {
//     //             stack.push_back(c); // Push opening braces onto stack
//     //         } else if (c == '}' || c == ']') {
//     //             if (stack.empty() || !isValidClosingBracket(stack.back(), c)) {
//     //                 return false; // Invalid closing brace
//     //             }
//     //             stack.pop_back(); // Pop matching opening brace
//     //         } else if (c == '\\') {
//     //             inEscape = true; // Set escape flag
//     //         }
//     //     }
//     // }

//     // return stack.empty(); // If stack is empty, all braces were closed properly


//      std::vector<char> stack; // Stack to track nested objects and arrays
//     bool inString = false;    // Flag to track if currently inside a string
//     bool inEscape = false;    // Flag to track if the previous character was an escape character

//     for (std::string::const_iterator it = jsonString.begin(); it != jsonString.end(); ++it) {
//         char c = *it;
//         if (inEscape) {
//             inEscape = false; // Skip next character after escape
//             continue;
//         }

//         if (c == '"') {
//             inString = !inString; // Toggle string flag
//         }

//         if (!inString) {
//             if (c == '{' || c == '[') {
//                 stack.push_back(c); // Push opening braces onto stack
//             } else if (c == '}' || c == ']') {
//                 if (stack.empty() || !isValidClosingBracket(stack.back(), c)) {
//                     throw std::runtime_error("Invalid JSON syntax");
//                 }
//                 stack.pop_back(); // Pop matching opening brace
//             } else if (c == '\\') {
//                 inEscape = true; // Set escape flag
//             }
//         }
//     }

//     if (!stack.empty()) {
//         throw std::runtime_error("Invalid JSON syntax");
//     }
// }



// bool HttpRequestParser::isValidClosingBracket(char opening, char closing) {
//     return (opening == '{' && closing == '}') || (opening == '[' && closing == ']');
// }


// std::string  HttpRequestParser::extractTag(const std::string& line) {
//     size_t start = line.find_first_of("<");
//     size_t end = line.find_first_of(">");

//     if (start != std::string::npos && end != std::string::npos && start < end) {
//         return line.substr(start + 1, end - start - 1);
//     }

//     return "";
// }

// bool HttpRequestParser::validateXml(const std::string& xmlString) {
//     // std::istringstream iss(xmlString);
//     // std::string line;
//     // std::vector<std::string> tags;
//     // std::string rootTag;

//     // while (std::getline(iss, line)) {
//     //     std::string tag = extractTag(line);
//     //     if (tag.empty()) {
//     //         continue; // Skip empty lines
//     //     }

//     //     if (tags.empty()) {
//     //         rootTag = tag;
//     //         tags.push_back(tag);
//     //     } else {
//     //         std::string expectedClosingTag = "</" + tags.back() + ">";
//     //         if (line == expectedClosingTag) {
//     //             tags.pop_back();
//     //         } else {
//     //             std::string openingTag = "<" + tag + ">";
//     //             if (line.substr(0, openingTag.size()) == openingTag) {
//     //                 tags.push_back(tag);
//     //             } else {
//     //                 std::cerr << "Invalid XML structure: " << line << std::endl;
//     //                 return false;
//     //             }
//     //         }
//     //     }
//     // }

//     // if (!tags.empty()) {
//     //     std::cerr << "Unclosed tags in XML" << std::endl;
//     //     return false;
//     // }

//     // std::cout << "Root tag: " << rootTag << std::endl;
//     // return true;



//      std::istringstream iss(xmlString);
//     std::string line;
//     std::vector<std::string> tags;
//     std::string rootTag;

//     while (std::getline(iss, line)) {
//         std::string tag = extractTag(line);
//         if (tag.empty()) {
//             continue; // Skip empty lines
//         }

//         if (tags.empty()) {
//             rootTag = tag;
//             tags.push_back(tag);
//         } else {
//             std::string expectedClosingTag = "</" + tags.back() + ">";
//             if (line == expectedClosingTag) {
//                 tags.pop_back();
//             } else {
//                 std::string openingTag = "<" + tag + ">";
//                 if (line.substr(0, openingTag.size()) == openingTag) {
//                     tags.push_back(tag);
//                 } else {
//                     throw std::runtime_error("Invalid XML structure");
//                 }
//             }
//         }
//     }

//     if (!tags.empty()) {
//         throw std::runtime_error("Unclosed tags in XML");
//     }

//     std::cout << "Root tag: " << rootTag << std::endl;
// }


// bool HttpRequestParser::isValidKeyValuePair(const std::string& pair) {
//     // size_t equalPos = pair.find('=');
//     // if (equalPos == std::string::npos || equalPos == 0 || equalPos == pair.length() - 1) {
//     //     return false; // Missing or invalid '=' sign
//     // }

//     // std::string key = pair.substr(0, equalPos);
//     // std::string value = pair.substr(equalPos + 1);

//     // if (!isValidUrlEncoded(key) || !isValidUrlEncoded(value)) {
//     //     return false; // Key or value is not URL-encoded
//     // }

//     // return true;
//         size_t equalPos = pair.find('=');
//     if (equalPos == std::string::npos || equalPos == 0 || equalPos == pair.length() - 1) {
//         return false; // Missing or invalid '=' sign
//     }

//     std::string key = pair.substr(0, equalPos);
//     std::string value = pair.substr(equalPos + 1);

//     if (!isValidUrlEncoded(key) || !isValidUrlEncoded(value)) {
//         return false; // Key or value is not URL-encoded
//     }

//     return true;
// }

// bool HttpRequestParser::isValidUrlEncoded(const std::string& str) {
//     for (char c : str) {
//         if (!isalnum(c) && c != '-' && c != '.' && c != '_' && c != '~' && c != '%' && c != '=') {
//             return false; // Invalid character for URL encoding
//         }
//     }
//     return true;
// }

// bool HttpRequestParser::validateFormData(const std::string& formData) {
//     // std::istringstream iss(formData);
//     // std::string pair;

//     // while (std::getline(iss, pair, '&')) {
//     //     if (!isValidKeyValuePair(pair)) {
//     //         return false; // Invalid key-value pair
//     //     }
//     // }

//     // return true; // All key-value pairs are valid
//         std::istringstream iss(formData);
//     std::string pair;

//     while (std::getline(iss, pair, '&')) {
//         if (!isValidKeyValuePair(pair)) {
//             throw std::runtime_error("Invalid key-value pair");
//         }
//     }
// }

// bool HttpRequestParser::isPrintableAscii(char c) {
//     return c >= 32 && c <= 126; // Range of printable ASCII characters
// }

// bool HttpRequestParser::validatePlainText(const std::string& text) {
//     for (char c : text) {
//         if (!isPrintableAscii(c)) {
//             return false; // Contains non-printable ASCII character
//         }
//     }
//     return true; // All characters are valid
// }


// // Helper function to split multipart data into parts
// static std::vector<std::string> splitMultipartData(const std::string& data, const std::string& boundary) {
//     // std::vector<std::string> parts;
//     // size_t pos = 0;
//     // size_t nextPos = 0;

//     // while ((nextPos = data.find(boundary, pos)) != std::string::npos) {
//     //     parts.push_back(data.substr(pos, nextPos - pos));
//     //     pos = nextPos + boundary.length();
//     // }

//     // return parts;
//     std::vector<std::string> parts;
//     size_t pos = 0;
//     size_t nextPos = 0;

//     while ((nextPos = data.find(boundary, pos)) != std::string::npos) {
//         parts.push_back(data.substr(pos, nextPos - pos));
//         pos = nextPos + boundary.length();
//     }

//     return parts;
// }

// bool HttpRequestParser::validateMultipartData(const std::string& data, const std::string& boundary) {
//     // // Split the data into parts based on the boundary
//     // std::vector<std::string> parts = splitMultipartData(data, boundary);

//     // // Check each part for validity (for simplicity, just check if it contains any data)
//     // for (const std::string& part : parts) {
//     //     if (part.empty()) {
//     //         return false; // Invalid part (empty)
//     //     }
//     // }

//     // return true; // All parts are valid
//         // Split the data into parts based on the boundary
//     std::string boundary = "example_boundary"; // Modify this with your boundary
//     std::vector<std::string> parts = splitMultipartData(data, boundary);

//     // Check each part for validity (for simplicity, just check if it contains any data)
//     for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it) {
//         if (it->empty()) {
//             throw std::runtime_error("Invalid part (empty)");
//         }
//     }
// }

// bool  HttpRequestParser::validateBinaryData (std::string data) {
//         return !data.empty();
// };

