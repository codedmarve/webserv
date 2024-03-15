#include "../inc/HttpRequestParser.hpp"

void HttpRequestParser::parseChunkedBody(const std::string &chunkedBody) {
    size_t pos = 0;
    while (pos < chunkedBody.size()) {
        size_t chunkSizePos = chunkedBody.find("\r\n", pos);
        if (chunkSizePos == std::string::npos)
            throw std::invalid_argument("Invalid chunked body");

        std::string chunkSizeStr = chunkedBody.substr(pos, chunkSizePos - pos);
        size_t chunkSize;
        std::istringstream iss(chunkSizeStr);
        if (!(iss >> std::hex >> chunkSize))
            throw std::invalid_argument("Invalid chunk size format");

        pos = chunkSizePos + 2; // Move past "\r\n"

        if (chunkSize == 0) {
            // Last chunk
            break;
        }

        size_t nextChunkPos = pos + chunkSize;
        if (nextChunkPos > chunkedBody.size()) {
            throw std::invalid_argument("Incomplete chunked body");
        }

        // Append this chunk to the body
        body_ += chunkedBody.substr(pos, chunkSize);

        pos = nextChunkPos + 2; // Move past chunk data and "\r\n"
    }
}



// #include <iostream>
// #include <string>
// #include <sstream>
// #include <stdexcept>
// #include <vector>
// #include <regex>

// class HttpRequestBodyParser {
// private:
//     std::string body_;
//     bool isChunked_;

//     unsigned int hexToDecimal(const std::string& hex) {
//         std::stringstream ss;
//         ss << std::hex << hex;
//         unsigned int decimal;
//         if (!(ss >> decimal)) {
//             throw std::invalid_argument("Invalid hex value");
//         }
//         return decimal;
//     }

//     void parseChunkedBody(const std::string& chunkedBody) {
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

// public:
//     HttpRequestBodyParser(const std::string& body, bool isChunked)
//         : body_(body), isChunked_(isChunked) {}

//     std::string parseAndValidateBody() {
//         if (isChunked_) {
//             // Parse chunked body
//             parseChunkedBody(body_);
//         } else {
//             // Non-chunked body, no parsing needed
//             body_ = body_;
//         }

//         // Additional validation or parsing logic based on content type
//         std::string contentType = "application/json"; // Example content type (can be obtained from headers)
//         if (contentType.find("application/json") != std::string::npos) {
//             // JSON body
//             // Additional JSON validation or parsing can be done here
//             // For simplicity, just return the JSON string as is
//             return body_;
//         } else if (contentType.find("text/plain") != std::string::npos) {
//             // Plain text body
//             // Additional processing for plain text can be added here
//             return body_;
//         } else {
//             // Unsupported content type
//             throw std::runtime_error("Unsupported content type: " + contentType);
//         }
//     }
// };

// int main() {
//     // Example HTTP request body with JSON content
//     std::string jsonBody = "{ \"name\": \"John Doe\", \"age\": 30 }";
//     bool isChunked = false;

//     HttpRequestBodyParser jsonParser(jsonBody, isChunked);
//     try {
//         std::string parsedJsonBody = jsonParser.parseAndValidateBody();
//         std::cout << "Parsed JSON body:\n" << parsedJsonBody << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
//     }

//     // Example HTTP request body with plain text content
//     std::string textBody = "This is a plain text body.";
//     HttpRequestBodyParser textParser(textBody, isChunked);
//     try {
//         std::string parsedTextBody = textParser.parseAndValidateBody();
//         std::cout << "Parsed text body:\n" << parsedTextBody << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error parsing text body: " << e.what() << std::endl;
//     }

//     return 0;
// }










#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <regex>

class HttpRequestBodyParser {
private:
    std::string body_;
    bool isChunked_;

    unsigned int hexToDecimal(const std::string& hex) {
        std::stringstream ss;
        ss << std::hex << hex;
        unsigned int decimal;
        if (!(ss >> decimal)) {
            throw std::invalid_argument("Invalid hex value");
        }
        return decimal;
    }

    void parseChunkedBody(const std::string& chunkedBody) {
        std::istringstream iss(chunkedBody);
        std::string line;
        std::string decodedBody;

        while (std::getline(iss, line)) {
            // Convert chunk size from hex to decimal
            unsigned int chunkSize = hexToDecimal(line);
            
            // Check for valid CRLF after chunk size
            if (iss.get() != '\r' || iss.get() != '\n') {
                throw std::runtime_error("Invalid CRLF after chunk size");
            }

            if (chunkSize == 0) {
                // Last chunk, end of body
                break;
            }

            std::string chunkData;
            for (unsigned int i = 0; i < chunkSize; ++i) {
                char c;
                if (!(iss.get(c))) {
                    throw std::runtime_error("Invalid chunked body format");
                }
                chunkData += c;
            }

            // Check for valid CRLF after chunk data
            if (iss.get() != '\r' || iss.get() != '\n') {
                throw std::runtime_error("Invalid CRLF after chunk data");
            }

            // Append chunk data to decoded body
            decodedBody += chunkData;
        }

        body_ = decodedBody;
    }

    void validateJson(const std::string& jsonBody) {
        // Example JSON validation (can be extended)
        // Here, we're checking if it's valid JSON syntax
        try {
            std::istringstream iss(jsonBody);
            iss >> std::ws;
            if (iss.peek() == std::char_traits<char>::eof()) {
                throw std::runtime_error("Empty JSON body");
            }
            iss.unget();
            if (!(iss >> std::ws).eof()) {
                throw std::runtime_error("Invalid JSON syntax");
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid JSON body: " + std::string(e.what()));
        }
    }

    void validateXml(const std::string& xmlBody) {
        // Example XML validation (can be extended)
        // Here, we're checking if it's valid XML syntax
        try {
            // Add XML validation logic here
            // For simplicity, we're not doing detailed XML validation
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid XML body: " + std::string(e.what()));
        }
    }

    void validateFormData(const std::string& formData) {
        // Example form data validation (can be extended)
        // Here, we're assuming it's valid if it's not empty
        if (formData.empty()) {
            throw std::runtime_error("Empty form data");
        }
    }

    void validatePlainText(const std::string& plainText) {
        // Example plain text validation (can be extended)
        // Here, we're assuming it's valid if it's not empty
        if (plainText.empty()) {
            throw std::runtime_error("Empty plain text body");
        }
    }

    void validateMultipartData(const std::string& multipartData) {
        // Example multipart data validation (can be extended)
        // Here, we're assuming it's valid if it's not empty
        if (multipartData.empty()) {
            throw std::runtime_error("Empty multipart data");
        }
    }

    void validateBinaryData(const std::string& binaryData) {
        // Example binary data validation (can be extended)
        // Here, we're assuming it's valid if it's not empty
        if (binaryData.empty()) {
            throw std::runtime_error("Empty binary data");
        }
    }

public:
    HttpRequestBodyParser(const std::string& body, bool isChunked)
        : body_(body), isChunked_(isChunked) {}

    std::string parseAndValidateBody(const std::string& contentType) {
        if (isChunked_) {
            // Parse chunked body
            parseChunkedBody(body_);
        } else {
            // Non-chunked body, no parsing needed
            body_ = body_;
        }

        // Additional validation or parsing logic based on content type
        if (contentType.find("application/json") != std::string::npos) {
            // JSON body
            validateJson(body_);
            return body_;
        } else if (contentType.find("application/xml") != std::string::npos) {
            // XML body
            validateXml(body_);
            return body_;
        } else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
            // Form data
            validateFormData(body_);
            return body_;
        } else if (contentType.find("text/plain") != std::string::npos) {
            // Plain text body
            validatePlainText(body_);
            return body_;
        } else if (contentType.find("multipart/form-data") != std::string::npos) {
            // Multipart data
            validateMultipartData(body_);
            return body_;
        } else if (contentType.find("application/octet-stream") != std::string::npos) {
            // Binary data
            validateBinaryData(body_);
            return body_;
        } else {
            // Unsupported content type
            throw std::runtime_error("Unsupported content type: " + contentType);
        }
    }
};

int main() {
    // Example HTTP request body with JSON content
    std::string jsonBody = "{ \"name\": \"John Doe\", \"age\": 30 }";
    bool isChunked = false;

    HttpRequestBodyParser jsonParser(jsonBody, isChunked);
    try {
        std::string parsedJsonBody = jsonParser.parseAndValidateBody("application/json");
        std::cout << "Parsed JSON body:\n" << parsedJsonBody << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
    }

    // Example HTTP request body with plain text content
    std::string textBody = "This is a plain text body.";
    HttpRequestBodyParser textParser(textBody, isChunked);
    try {
        std::string parsedTextBody = textParser.parseAndValidateBody("text/plain");
        std::cout << "Parsed text body:\n" << parsedTextBody << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing text body: " << e.what() << std::endl;
    }

    // Add more examples with different content types if needed

    return 0;
}







#include <iostream>
#include <string>
#include <vector>

class JsonValidator {
public:
    static bool validateJson(const std::string& jsonString) {
        std::vector<char> stack; // Stack to track nested objects and arrays
        bool inString = false;    // Flag to track if currently inside a string
        bool inEscape = false;    // Flag to track if the previous character was an escape character

        for (char c : jsonString) {
            if (inEscape) {
                inEscape = false; // Skip next character after escape
                continue;
            }

            if (c == '"') {
                inString = !inString; // Toggle string flag
            }

            if (!inString) {
                if (c == '{' || c == '[') {
                    stack.push_back(c); // Push opening braces onto stack
                } else if (c == '}' || c == ']') {
                    if (stack.empty() || !isValidClosingBracket(stack.back(), c)) {
                        return false; // Invalid closing brace
                    }
                    stack.pop_back(); // Pop matching opening brace
                } else if (c == '\\') {
                    inEscape = true; // Set escape flag
                }
            }
        }

        return stack.empty(); // If stack is empty, all braces were closed properly
    }

private:
    static bool isValidClosingBracket(char opening, char closing) {
        return (opening == '{' && closing == '}') || (opening == '[' && closing == ']');
    }
};

int main() {
    // Valid JSON
    std::string validJson = "{\"name\": \"John Doe\", \"age\": 30, \"active\": true}";
    if (JsonValidator::validateJson(validJson)) {
        std::cout << "Valid JSON" << std::endl;
    } else {
        std::cout << "Invalid JSON" << std::endl;
    }

    // Invalid JSON with mismatched braces
    std::string invalidJson = "{\"name\": \"John Doe\", \"age\": 30, \"active\": true}";
    if (JsonValidator::validateJson(invalidJson)) {
        std::cout << "Valid JSON" << std::endl;
    } else {
        std::cout << "Invalid JSON" << std::endl;
    }

    return 0;
}











#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class XmlValidator {
public:
    static bool validateXml(const std::string& xmlString) {
        std::istringstream iss(xmlString);
        std::string line;
        std::vector<std::string> tags;
        std::string rootTag;

        while (std::getline(iss, line)) {
            std::string tag = extractTag(line);
            if (tag.empty()) {
                continue; // Skip empty lines
            }

            if (tags.empty()) {
                rootTag = tag;
                tags.push_back(tag);
            } else {
                std::string expectedClosingTag = "</" + tags.back() + ">";
                if (line == expectedClosingTag) {
                    tags.pop_back();
                } else {
                    std::string openingTag = "<" + tag + ">";
                    if (line.substr(0, openingTag.size()) == openingTag) {
                        tags.push_back(tag);
                    } else {
                        std::cerr << "Invalid XML structure: " << line << std::endl;
                        return false;
                    }
                }
            }
        }

        if (!tags.empty()) {
            std::cerr << "Unclosed tags in XML" << std::endl;
            return false;
        }

        std::cout << "Root tag: " << rootTag << std::endl;
        return true;
    }

private:
    static std::string extractTag(const std::string& line) {
        size_t start = line.find_first_of("<");
        size_t end = line.find_first_of(">");

        if (start != std::string::npos && end != std::string::npos && start < end) {
            return line.substr(start + 1, end - start - 1);
        }

        return "";
    }
};

int main() {
    std::string xmlBody = "<root><name>John Doe</name><age>30</age></root>";

    if (XmlValidator::validateXml(xmlBody)) {
        std::cout << "XML is valid" << std::endl;
    } else {
        std::cout << "XML is invalid" << std::endl;
    }

    return 0;
}

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

class FormDataValidator {
public:
    static bool validateFormData(const std::string& formData) {
        std::istringstream iss(formData);
        std::string pair;

        while (std::getline(iss, pair, '&')) {
            if (!isValidKeyValuePair(pair)) {
                return false; // Invalid key-value pair
            }
        }

        return true; // All key-value pairs are valid
    }

private:
    static bool isValidKeyValuePair(const std::string& pair) {
        size_t equalPos = pair.find('=');
        if (equalPos == std::string::npos || equalPos == 0 || equalPos == pair.length() - 1) {
            return false; // Missing or invalid '=' sign
        }

        std::string key = pair.substr(0, equalPos);
        std::string value = pair.substr(equalPos + 1);

        if (!isValidUrlEncoded(key) || !isValidUrlEncoded(value)) {
            return false; // Key or value is not URL-encoded
        }

        return true;
    }

    static bool isValidUrlEncoded(const std::string& str) {
        for (char c : str) {
            if (!isalnum(c) && c != '-' && c != '.' && c != '_' && c != '~' && c != '%' && c != '=') {
                return false; // Invalid character for URL encoding
            }
        }
        return true;
    }
};

int main() {
    // Valid form data
    std::string validFormData = "name=John%20Doe&age=30&email=john%40example.com";
    if (FormDataValidator::validateFormData(validFormData)) {
        std::cout << "Valid form data" << std::endl;
    } else {
        std::cout << "Invalid form data" << std::endl;
    }

    // Invalid form data with invalid characters
    std::string invalidFormData = "name=JohnDoe&age=30&email=john@example.com";
    if (FormDataValidator::validateFormData(invalidFormData)) {
        std::cout << "Valid form data" << std::endl;
    } else {
        std::cout << "Invalid form data" << std::endl;
    }

    return 0;
}

#include <iostream>
#include <string>
#include <cctype>

class PlainTextValidator {
public:
    static bool validatePlainText(const std::string& text) {
        for (char c : text) {
            if (!isPrintableAscii(c)) {
                return false; // Contains non-printable ASCII character
            }
        }
        return true; // All characters are valid
    }

private:
    static bool isPrintableAscii(char c) {
        return c >= 32 && c <= 126; // Range of printable ASCII characters
    }
};

int main() {
    // Valid plain text
    std::string validText = "Hello, this is valid plain text!";
    if (PlainTextValidator::validatePlainText(validText)) {
        std::cout << "Valid plain text" << std::endl;
    } else {
        std::cout << "Invalid plain text" << std::endl;
    }

    // Invalid plain text with non-printable characters
    std::string invalidText = "Invalid \x01\x02\x03 text";
    if (PlainTextValidator::validatePlainText(invalidText)) {
        std::cout << "Valid plain text" << std::endl;
    } else {
        std::cout << "Invalid plain text" << std::endl;
    }

    return 0;
}




#include <iostream>
#include <string>
#include <vector>

class MultipartDataValidator {
public:
    static bool validateMultipartData(const std::string& data, const std::string& boundary) {
        // Split the data into parts based on the boundary
        std::vector<std::string> parts = splitMultipartData(data, boundary);

        // Check each part for validity (for simplicity, just check if it contains any data)
        for (const std::string& part : parts) {
            if (part.empty()) {
                return false; // Invalid part (empty)
            }
        }

        return true; // All parts are valid
    }

private:
    // Helper function to split multipart data into parts
    static std::vector<std::string> splitMultipartData(const std::string& data, const std::string& boundary) {
        std::vector<std::string> parts;
        size_t pos = 0;
        size_t nextPos = 0;

        while ((nextPos = data.find(boundary, pos)) != std::string::npos) {
            parts.push_back(data.substr(pos, nextPos - pos));
            pos = nextPos + boundary.length();
        }

        return parts;
    }
};

int main() {
    // Example multipart data and boundary
    std::string multipartData = "--boundary\r\nContent-Type: text/plain\r\n\r\nThis is part 1\r\n--boundary\r\nContent-Type: application/json\r\n\r\n{\"key\": \"value\"}\r\n--boundary--\r\n";
    std::string boundary = "--boundary";

    if (MultipartDataValidator::validateMultipartData(multipartData, boundary)) {
        std::cout << "Valid multipart data" << std::endl;
    } else {
        std::cout << "Invalid multipart data" << std::endl;
    }

    return 0;
}


#include <iostream>
#include <string>

class BinaryDataValidator {
public:
    static bool validateBinaryData(const std::string& data) {
        return !data.empty();
    }
};

int main() {
    // Example binary data
    std::string binaryData = "\x00\xFF\x12\x34\x56\x78";

    if (BinaryDataValidator::validateBinaryData(binaryData)) {
        std::cout << "Valid binary data" << std::endl;
    } else {
        std::cout << "Invalid binary data" << std::endl;
    }

    return 0;
}
