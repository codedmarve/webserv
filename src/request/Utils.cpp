#include "../../inc/HttpRequestParser.hpp"


void HttpRequestParser::printRequest(HttpRequestParser parser) {
    ;
    try {
        // parser.parseRequest(request);
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getURI() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;
        std::cout << "Headers:" << std::endl;
        const std::map<std::string, std::string>& headers = parser.getHeaders();
        std::map<std::string, std::string>::const_iterator it;
        for (it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }
        std::cout << "Body: " << parser.getBody() << std::endl;
        std::cout << "-------------------------------\n";
    } catch (const std::exception& e) {
        std::cerr << "Error parsing request: " << e.what() << std::endl;
    }
}

bool HttpRequestParser::isMethodCharValid(char ch) const {
    // According to RFC 7229, valid characters for method are:
    return (ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' ||
            ch == '*' || ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' ||
            ch == '`' || ch == '|' || ch == '~' || std::isdigit(ch) || std::isalpha(ch));
}



bool HttpRequestParser::isAlpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool HttpRequestParser::isDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool HttpRequestParser::isAlphaNum(char c) {
    return isAlpha(c) || isDigit(c);
}

void HttpRequestParser::print_uri_extracts() {
    std::cout << "uri: " << uri_ << "\n" << std::endl;
    std::cout << "scheme: " << scheme_ << std::endl;
    std::cout << "authority: " << authority_ << std::endl;
    std::cout << "path: " << path_ << std::endl;
    std::cout << "query: " << query_ << std::endl;
    std::cout << "fragment: " << frag_ << std::endl;
}


bool HttpRequestParser::isUnreserved(char c) {
    return isAlphaNum(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool HttpRequestParser::isSubDelim(char c) {
    return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
}

bool HttpRequestParser::isHexDigit(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

std::string HttpRequestParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

unsigned int HttpRequestParser::hexToDecimal(const std::string& hex) {
    std::stringstream ss;
    ss << std::hex << hex;
    unsigned int decimal;
    if (!(ss >> decimal)) {
        throw std::invalid_argument("Invalid hex value");
    }
    return decimal;
}
