#include "../inc/HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &request) {
    size_t pos;
    std::string headerLines;
    size_t endOfFirstLine;
    std::string headers;
    std::string requestLine;

    pos = request.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        headerLines = request.substr(0, pos);
        body_ = request.substr(pos + 4);
        endOfFirstLine = headerLines.find("\r\n");
        
        headers = headerLines.substr(endOfFirstLine + 2);

        if (endOfFirstLine != std::string::npos)
            requestLine = headerLines.substr(0, endOfFirstLine);
        else
            return 400;

        try {
            parseRequestLine(requestLine);
            parseHeaders(headers);

            return 200;
        } catch (const std::invalid_argument& e) {
            return 405; // Method Not Allowed
        }

    } else
        throw std::invalid_argument("Invalid request format");
    return 200;
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







//  const int INVALID_SCHEME = 400;
//     const int INVALID_USERINFO = 401;
//     const int INVALID_HOST = 402;
//     const int INVALID_PORT = 403;
//     const int INVALID_PATH = 404;
//     const int INVALID_QUERY = 405;
//     const int INVALID_FRAGMENT = 406;
