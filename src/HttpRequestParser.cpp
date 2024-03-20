#include "../inc/HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {}

int HttpRequestParser::parseRequest(const std::string &buffer) {
    size_t pos;
    std::string headerLines;
    size_t endOfFirstLine;
    std::string headers;
    std::string requestLine;

    // init data
    request_ = "";
    isChunked_ = false;
    request_ += buffer;

    pos = request_.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        headerLines = request_.substr(0, pos + 2);

        body_ = request_.substr(pos + 4);
        endOfFirstLine = headerLines.find("\r\n");
        

        try {
            if (endOfFirstLine != std::string::npos)
            {
                requestLine = headerLines.substr(0, endOfFirstLine);
                headers = headerLines.substr(endOfFirstLine + 2);
                parseRequestLine(requestLine);
                if (headers.empty() && authority_.empty())
                    return 400;
            }
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







