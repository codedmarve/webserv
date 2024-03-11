#include "../inc/HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &request) {
    size_t pos;
    std::string headerLines;
    size_t endOfFirstLine;
    std::string headers;
    std::string requestLine;

    pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        headerLines = request.substr(0, pos);
        body_ = request.substr(pos + 4);
        endOfFirstLine = headerLines.find("\r\n");
        headers = headerLines.substr(endOfFirstLine + 2);
        if (endOfFirstLine != std::string::npos)
            requestLine = headerLines.substr(0, endOfFirstLine);

        try {

            parseRequestLine(headerLines, endOfFirstLine);
            parseHeaders(headers);

            return 200; // Successfully parsed
        } catch (const std::invalid_argument& e) {
            // Invalid method or character in method
            return 405; // Method Not Allowed
        }

    } else
        throw std::invalid_argument("Invalid request format");
    return 200;
}

void HttpRequestParser::parseHeaders(const std::string &headerLines) {
    std::istringstream iss(headerLines);
    std::string line;
    while (std::getline(iss, line, '\r')) {
        if (!line.empty()) {
            size_t pos = line.find(": ");
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 2);
                headers_[key] = value;
            }
        }
    }
}

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

void HttpRequestParser::parseContentLength() {
    std::map<std::string, std::string>::iterator it = headers_.find("Content-Length");
    if (it != headers_.end()) {
        std::istringstream iss(it->second);
        size_t contentLength;
        if (!(iss >> contentLength))
            throw std::invalid_argument("Invalid Content-Length value");

        if (contentLength > body_.size())
            throw std::invalid_argument("Incomplete body");

        body_ = body_.substr(0, contentLength);
    }
}

std::string HttpRequestParser::getMethod() const {
    return method_;
}

std::string HttpRequestParser::getURI() const {
    return uri_;
}

std::string HttpRequestParser::getProtocol() const {
    return protocol_;
}

std::map<std::string, std::string> HttpRequestParser::getHeaders() const {
    return headers_;
}

std::string HttpRequestParser::getBody() const {
    return body_;
}

void HttpRequestParser::printRequest(const std::string& request) {
    HttpRequestParser parser;
    try {
        parser.parseRequest(request);
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

int HttpRequestParser::extractRequestLine(std::string requestLine) {
    std::istringstream iss(requestLine);
    size_t methodEnd;
    size_t uriStart;
    size_t protocolStart;

    methodEnd = requestLine.find(' ');
    uriStart = methodEnd + 1;
    protocolStart = requestLine.rfind(' '); 

    // Check if both URI and protocol positions are valid
    if (methodEnd == std::string::npos || protocolStart == std::string::npos || protocolStart <= uriStart)
        return 400;

    // Extract METHOD, URI and Protocol
    method_ = requestLine.substr(0, methodEnd);
    uri_ = requestLine.substr(uriStart, protocolStart - uriStart);
    protocol_ = requestLine.substr(protocolStart + 1);
    return 200;
}

int HttpRequestParser::parseRequestLine(std::string headerLines, size_t eofFirstLine) {
    int status = 0;

    if (eofFirstLine != std::string::npos) {
        std::string requestLine = headerLines.substr(0, eofFirstLine);
        extractRequestLine(requestLine);

        try {
            status = parseMethod();
            if (status == 200) {
                std::cout << "METHOD is valid!" << std::endl;
            } else {
                std::cout << "METHOD is invalid -> Error code: " << status << std::endl;
            }
            status = validateURI(uri_);
            if (status == 200) {
                std::cout << "URI is valid!" << std::endl;
            } else {
                std::cout << "URI is invalid -> Error code: " << status << std::endl;
            }

            return 200; // Successfully parsed
        } catch (const std::invalid_argument& e) {
            // Invalid method or character in method
            return 405; // Method Not Allowed
        }
    } else {
        return 400;
    }
}


int HttpRequestParser::parseMethod() {
    // Set of valid HTTP methods
    static const std::string validMethodsArray[] = {
        "GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD", "TRACE", "CONNECT"
    };
    static const std::set<std::string> validMethods(validMethodsArray, validMethodsArray + sizeof(validMethodsArray) / sizeof(validMethodsArray[0]));

    // Check if the method is valid and in uppercase
    if (validMethods.find(method_) == validMethods.end()) {
        // throw std::invalid_argument("Invalid HTTP method");
        return 501;
    }

    // Check each character for validity
    for (std::string::const_iterator it = method_.begin(); it != method_.end(); ++it) {
        if (!isMethodCharValid(*it)) {
            // throw std::invalid_argument("Invalid character in HTTP method");
            return 400;
        }
    }
    return 200; // OK
}

bool HttpRequestParser::isMethodCharValid(char ch) const {
    // According to RFC 7230, valid characters for method are:
    return (ch == '!' || ch == '#' || ch == '$' || ch == '%' || ch == '&' || ch == '\'' ||
            ch == '*' || ch == '+' || ch == '-' || ch == '.' || ch == '^' || ch == '_' ||
            ch == '`' || ch == '|' || ch == '~' || std::isdigit(ch) || std::isalpha(ch));
}





bool HttpRequestParser::extractComponents(const std::string& uri, std::string& scheme, std::string& authority, std::string& path, std::string& query, std::string& fragment) {
    size_t schemeEnd = uri.find(':');
    if (schemeEnd == std::string::npos) {
        return false; // Missing scheme
    }

    scheme = uri.substr(0, schemeEnd);
    if (schemeEnd + 3 < uri.length() && uri.substr(schemeEnd, 3) == "://") {
        size_t authorityStart = schemeEnd + 3;
        size_t authorityEnd = uri.find_first_of("/?#", authorityStart);
        if (authorityEnd == std::string::npos) {
            authorityEnd = uri.length();
        }

        authority = uri.substr(authorityStart, authorityEnd - authorityStart);

        size_t pathStart = authorityEnd;
        size_t queryStart = uri.find('?', pathStart);
        size_t fragmentStart = uri.find('#', pathStart);

        if (queryStart != std::string::npos) {
            path = uri.substr(pathStart, queryStart - pathStart);
        } else if (fragmentStart != std::string::npos) {
            path = uri.substr(pathStart, fragmentStart - pathStart);
        } else {
            path = uri.substr(pathStart);
        }

        if (queryStart != std::string::npos) {
            query = uri.substr(queryStart + 1, fragmentStart - (queryStart + 1));
        }

        if (fragmentStart != std::string::npos) {
            fragment = uri.substr(fragmentStart + 1);
        }

        return true;
    }

    return false; // Missing authority
}

bool HttpRequestParser::isValidScheme(const std::string& scheme) {
    if (scheme.empty() || !isAlpha(scheme[0])) {
        return false; // Scheme must start with a letter
    }

    const std::string validSchemeChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-.";
    for (size_t i = 0; i < scheme.length(); ++i) {
        if (!isAlphaNum(scheme[i]) && validSchemeChars.find(scheme[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool HttpRequestParser::isValidAuthority(const std::string& authority) {
    size_t userinfoEnd = authority.find('@');
    std::string userinfo = userinfoEnd != std::string::npos ? authority.substr(0, userinfoEnd) : "";

    size_t hostStart = userinfoEnd != std::string::npos ? userinfoEnd + 1 : 0;
    size_t portStart = authority.find(':', hostStart);
    std::string host = portStart != std::string::npos ? authority.substr(hostStart, portStart - hostStart) : authority.substr(hostStart);

    std::string port = (portStart != std::string::npos && portStart + 1 < authority.length()) ? authority.substr(portStart + 1) : "";

    const std::string validAuthorityChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:";
    for (size_t i = 0; i < userinfo.length(); ++i) {
        if (!isAlphaNum(userinfo[i]) && validAuthorityChars.find(userinfo[i]) == std::string::npos) {
            return false;
        }
    }

    for (size_t i = 0; i < host.length(); ++i) {
        if (!isAlphaNum(host[i]) && validAuthorityChars.find(host[i]) == std::string::npos) {
            return false;
        }
    }

    for (size_t i = 0; i < port.length(); ++i) {
        if (!isDigit(port[i])) {
            return false;
        }
    }

    return true;
}

bool HttpRequestParser::isValidPath(const std::string& path) {
    const std::string validPathChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
    if (!path.empty() && path[0] != '/') {
        return false; // Path must start with '/'
    }

    for (size_t i = 0; i < path.length(); ++i) {
        if (!isAlphaNum(path[i]) && validPathChars.find(path[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool HttpRequestParser::isValidQuery(const std::string& query) {
    const std::string validQueryChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
    if (!query.empty() && query[0] != '?') {
        return false; // Query must start with '?'
    }

    for (size_t i = 0; i < query.length(); ++i) {
        if (!isAlphaNum(query[i]) && validQueryChars.find(query[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool HttpRequestParser::isValidFragment(const std::string& fragment) {
    const std::string validFragmentChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";
    if (!fragment.empty() && fragment[0] != '#') {
        return false; // Fragment must start with '#'
    }

    for (size_t i = 0; i < fragment.length(); ++i) {
        if (!isAlphaNum(fragment[i]) && validFragmentChars.find(fragment[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
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

bool HttpRequestParser::validateURI(const std::string& uri) {
    if (uri.empty()) {
        return false; // URI cannot be empty
    }

    // Split URI into components
    std::string scheme, authority, path, query, fragment;
    bool hasAuthority = extractComponents(uri, scheme, authority, path, query, fragment);

    // Validate each component
    if (!isValidScheme(scheme)) {
        return false;
    }

    if (hasAuthority && !isValidAuthority(authority)) {
        return false;
    }

    if (!isValidPath(path)) {
        return false;
    }

    if (!isValidQuery(query)) {
        return false;
    }

    if (!isValidFragment(fragment)) {
        return false;
    }

    return true; // All checks passed
}
