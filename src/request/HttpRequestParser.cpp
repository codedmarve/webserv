#include "../../inc/HttpRequestParser.hpp"

HttpRequestParser::HttpRequestParser() {
    body_offset_ = 0;
    chunk_size_ = 0;
    buffer_section_ = REQUEST_LINE;
    protocol_ = "HTTP/1.1";
    req_buffer_ = "";
    isChunked_ = false;
    gettimeofday(&start_tv_, NULL);
}


HttpRequestParser::~HttpRequestParser() {

}

int HttpRequestParser::parseRequest(std::string &buffer) {
    size_t httpStatus = 0;
    std::string headerLines;
    std::string headers;
    std::string requestLine;

    gettimeofday(&last_tv_, NULL);
    req_buffer_ += buffer;
    buffer.clear();

    /// @note This if/else logic depends on how we wanna handle buffer
    if (buffer_section_ == REQUEST_LINE) {
        httpStatus = parseRequestLine();
    } else if (buffer_section_ == HEADERS) {
        httpStatus = parseHeaders();
    } else if (buffer_section_ == SPECIAL_HEADERS) {
        httpStatus = checkSpecialHeaders();
    } else if (buffer_section_ == BODY) {
        httpStatus = parseBody();
    } else if (buffer_section_ == CHUNK) {
        httpStatus = parseChunkedBody();
    }

    /// @todo we'll sort this out when errorhandling is sorted
    // if (buffer_section_ == COMPLETE || httpStatus == 200) {
    //     buffer_section_ = COMPLETE;
    // } else if (buffer_section_ == ERROR || httpStatus != 200) {
    //     buffer_section_ = ERROR;
    // }

    return httpStatus;
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







