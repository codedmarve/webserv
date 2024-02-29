// #ifndef HTTP_REQUEST_PARSER_HPP
// #define HTTP_REQUEST_PARSER_HPP

// #include <string>
// #include <map>
// #include <iostream>
// #include <cstdlib>
// #include <sstream> 
// #include <vector>

// class HttpRequestParser {
// private:
//     std::string method_;
//     std::string target_;
//     std::string protocol_;
//     std::map<std::string, std::string> headers_;
//     std::string body_;
//     int content_length_;

//     void parseHeaders(const std::string &headerLines);
//     void parseContentLength();  // Changed return type

// public:
//     HttpRequestParser();
//     ~HttpRequestParser();

//     int parseRequest(const std::string &request);

//     std::string getMethod() const;
//     std::string getTarget() const;
//     std::string getProtocol() const;
//     std::map<std::string, std::string> getHeaders() const;
//     std::string getBody() const;
//     int getContentLength() const;
//     void printRequest(const std::string& request);

// };

// #endif

#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>


class HttpRequestParser {
private:
    std::string method_;
    std::string target_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;

public:
    HttpRequestParser();

    void parseRequest(const std::string &request);
    void parseHeaders(const std::string &headerLines);
    void parseChunkedBody(const std::string &chunkedBody);
    void parseContentLength();

    std::string getMethod() const;
    std::string getTarget() const;
    std::string getProtocol() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

    void printRequest(const std::string& request);
};

#endif // HTTP_REQUEST_PARSER_HPP
