#include "../../inc/HttpRequestParser.hpp"

/// @todo
// handle error codes
// implement missing getters e.g get header by key

HttpRequestParser::HttpRequestParser() {
    body_offset_ = 0;
    chunk_size_ = 0;
    req_buffer_ = "";
    buffer_section_ = REQUEST_LINE;
    protocol_ = "HTTP/1.1";
    isChunked_ = false;
    gettimeofday(&start_tv_, NULL);
}

HttpRequestParser::~HttpRequestParser() {}

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
    }
    if (buffer_section_ == HEADERS) {
        httpStatus = parseHeaders();
    }
    if (buffer_section_ == SPECIAL_HEADERS) {
        httpStatus = checkSpecialHeaders();
    }
    if (buffer_section_ == BODY) {
        httpStatus = parseBody();
    } else if (buffer_section_ == CHUNK) {
        httpStatus = parseChunkedBody();
    }

    if (buffer_section_ == COMPLETE || httpStatus == 1000) {
        buffer_section_ = COMPLETE;
    } else if (buffer_section_ == ERROR || (httpStatus != 200 && httpStatus != 1000)) {
        buffer_section_ = ERROR;
    }

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

std::string &HttpRequestParser::getQuery() {
  return query_;
}

std::string &HttpRequestParser::getHeader(std::string key) {
  return headers_[key];
}

void HttpRequestParser::setTarget(std::string target) {
  target_ = target;
}

std::string &HttpRequestParser::getTarget() {
  return target_;
}



