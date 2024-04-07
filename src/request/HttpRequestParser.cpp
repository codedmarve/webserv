#include "../../inc/HttpRequestParser.hpp"

/// @note
// 200 indicates that parsing is ongoing and there is no error
// 100 indicates parsing is complete and successful
// any other number indicates an http req error or incomplete parsing state

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

    if (buffer_section_ == COMPLETE || httpStatus == 100) {
        buffer_section_ = COMPLETE;
    } else if (buffer_section_ == ERROR || (httpStatus != 200 && httpStatus != 100)) {
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

std::string &HttpRequestParser::getMethod() {
    return method_;
}

std::string &HttpRequestParser::getURI() {
    return uri_;
}

std::string &HttpRequestParser::getProtocol() {
    return protocol_;
}

std::map<std::string, std::string> HttpRequestParser::getHeaders() const {
    return headers_;
}

std::string &HttpRequestParser::getBody() {
    return body_;
}

std::string &HttpRequestParser::getPath() {
  return path_;
}

std::string &HttpRequestParser::getQuery() {
  return query_;
}

std::string &HttpRequestParser::getFragment() {
  return frag_;
}


std::string &HttpRequestParser::getHeader(std::string key) {
  return headers_[key];
}

void HttpRequestParser::setTarget(std::string target) {
  target_ = target;
}

std::string HttpRequestParser::getTarget() const {
  return target_;
}

size_t HttpRequestParser::getContentLength() {
  return length_;
}


bool HttpRequestParser::timeout() {
  if (buffer_section_ != COMPLETE) {
    buffer_section_ = ERROR;
    return true;
  }
  return false;
}

int HttpRequestParser::getStatus() {
  return buffer_section_;
}

time_t HttpRequestParser::get_start_timer_in_sec() {
  return start_tv_.tv_sec;
}

time_t HttpRequestParser::get_last_timer_in_sec() {
  return last_tv_.tv_sec;
}

