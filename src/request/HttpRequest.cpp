/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:15:33 by alappas           #+#    #+#             */
/*   Updated: 2024/05/28 23:15:34 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/HttpRequest.hpp"

HttpRequest::HttpRequest()
{
  body_offset_ = 0;
  chunk_size_ = 0;
  buffer_section_ = REQUEST_LINE;
  protocol_ = "HTTP/1.1";
  isChunked_ = false;
  gettimeofday(&start_tv_, NULL);
}

HttpRequest::~HttpRequest() {}

HttpRequest::HttpRequest(const HttpRequest &rhs)
    : method_(rhs.method_),
      uri_(rhs.uri_),
      scheme_(rhs.scheme_),
      authority_(rhs.authority_),
      path_(rhs.path_),
      query_(rhs.query_),
      frag_(rhs.frag_),
      target_(rhs.target_),
      protocol_(rhs.protocol_),
      headers_(rhs.headers_),
      body_(rhs.body_),
      req_buffer_(rhs.req_buffer_),
      length_(rhs.length_),
      chunk_size_(rhs.chunk_size_),
      isChunked_(rhs.isChunked_),
      body_offset_(rhs.body_offset_),
      start_tv_(rhs.start_tv_),
      last_tv_(rhs.last_tv_),
      buffer_section_(rhs.buffer_section_),
      chunk_status_(rhs.chunk_status_),
      uri_suffix_(rhs.uri_suffix_)
{
}

HttpRequest &HttpRequest::operator=(const HttpRequest &rhs)
{
  if (this != &rhs)
  {
    method_ = rhs.method_;
    uri_ = rhs.uri_;
    scheme_ = rhs.scheme_;
    authority_ = rhs.authority_;
    path_ = rhs.path_;
    query_ = rhs.query_;
    frag_ = rhs.frag_;
    target_ = rhs.target_;
    protocol_ = rhs.protocol_;
    headers_ = rhs.headers_;
    body_ = rhs.body_;
    req_buffer_ = rhs.req_buffer_;
    length_ = rhs.length_;
    chunk_size_ = rhs.chunk_size_;
    isChunked_ = rhs.isChunked_;
    body_offset_ = rhs.body_offset_;
    start_tv_ = rhs.start_tv_;
    last_tv_ = rhs.last_tv_;
    buffer_section_ = rhs.buffer_section_;
    chunk_status_ = rhs.chunk_status_;
    uri_suffix_ = rhs.uri_suffix_;
  }
  return *this;
}

int HttpRequest::parseRequest(std::string &buffer)
{
  size_t httpStatus = 0;
  std::string headerLines;
  std::string headers;
  std::string requestLine;

  gettimeofday(&last_tv_, NULL);
  req_buffer_.append(buffer);
  buffer.clear();

  try
  {
    if (buffer_section_ == REQUEST_LINE)
      httpStatus = parseRequestLine();
    if (buffer_section_ == HEADERS)
      httpStatus = parseHeaders();
    if (buffer_section_ == SPECIAL_HEADERS)
      httpStatus = checkSpecialHeaders();

    if (buffer_section_ == BODY)
      httpStatus = parseBody();
    else if (buffer_section_ == CHUNK)
      httpStatus = parseChunkedBody();

    if (buffer_section_ == COMPLETE || httpStatus == 100)
      buffer_section_ = COMPLETE;
    else if (buffer_section_ == ERROR || (httpStatus != 200 && httpStatus != 100))
      buffer_section_ = ERROR;
  }
  catch (const std::exception &e)
  {
    buffer_section_ = ERROR;
    std::cerr << CURSIVE_GRAY << "Exception in parseRequest: " << e.what() << RESET << std::endl;
    return 500;
  }

  return httpStatus;
}

void HttpRequest::parseContentLength()
{
  std::map<std::string, std::string>::iterator it = headers_.find("Content-Length");
  if (it != headers_.end())
  {
    std::istringstream iss(it->second);
    size_t contentLength;
    if (!(iss >> contentLength))
      throw std::invalid_argument("Invalid Content-Length value");

    if (contentLength > body_.size())
      throw std::invalid_argument("Incomplete body");

    body_ = body_.substr(0, contentLength);
  }
}

std::string &HttpRequest::getMethod()
{
  return method_;
}

std::string &HttpRequest::getURI()
{
  return uri_;
}

std::string &HttpRequest::getProtocol()
{
  return protocol_;
}

std::map<std::string, std::string> HttpRequest::getHeaders() const
{
  return headers_;
}

const std::string &HttpRequest::getBody() const
{
  return body_;
}

std::string &HttpRequest::getPath()
{
  return path_;
}

std::string &HttpRequest::getQuery()
{
  return query_;
}

std::string &HttpRequest::getFragment()
{
  return frag_;
}

std::string &HttpRequest::getHeader(std::string key)
{
  return headers_[key];
}

void HttpRequest::setTarget(std::string target)
{
  target_ = target;
}

std::string HttpRequest::getTarget() const
{
  return target_;
}

size_t HttpRequest::getContentLength()
{
  return length_;
}

bool HttpRequest::timeout()
{
  if (buffer_section_ != COMPLETE)
  {
    buffer_section_ = ERROR;
    return true;
  }
  return false;
}

int HttpRequest::getStatus()
{
  return buffer_section_;
}

time_t HttpRequest::get_start_timer_in_sec()
{
  return start_tv_.tv_sec;
}

time_t HttpRequest::get_last_timer_in_sec()
{
  return last_tv_.tv_sec;
}

void HttpRequest::print_uri_extracts()
{
  std::cout << "uri: " << uri_ << "\n"
            << std::endl;
  std::cout << "scheme: " << scheme_ << std::endl;
  std::cout << "authority: " << authority_ << std::endl;
  std::cout << "path: " << path_ << std::endl;
  std::cout << "query: " << query_ << std::endl;
  std::cout << "fragment: " << frag_ << std::endl;
}

void HttpRequest::printRequest(HttpRequest parser)
{
  try
  {
    std::cout << "Method: " << parser.getMethod() << std::endl;
    std::cout << "Target: " << parser.getURI() << std::endl;
    std::cout << "Protocol: " << parser.getProtocol() << std::endl;
    std::cout << "Headers:" << std::endl;
    const std::map<std::string, std::string> &headers = parser.getHeaders();
    std::map<std::string, std::string>::const_iterator it;
    for (it = headers.begin(); it != headers.end(); ++it)
    {
      std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << "Body: " << parser.getBody() << std::endl;
    std::cout << "-------------------------------\n";
  }
  catch (const std::exception &e)
  {
    std::cerr << "Error parsing request: " << e.what() << std::endl;
  }
}

std::string &HttpRequest::getBody()
{
  return body_;
}