// #ifndef HTTP_REQUEST_PARSER_HPP
// #define HTTP_REQUEST_PARSER_HPP

// #include <string>
// #include <map>
// #include <cstdlib>
// #include <sstream> 
// #include <iostream>
// #include <vector>

// class HttpRequestParser {
// public:
//     HttpRequestParser();
//     ~HttpRequestParser();

//     int parseRequest(const std::string &request);
//     int getContentLength();

//     std::string getMethod() const;
//     std::string getTarget() const;
//     std::string getProtocol() const;
//     std::map<std::string, std::string> getHeaders() const;
//     std::string getBody() const;

// private:
//     std::string method_;
//     std::string target_;
//     std::string protocol_;
//     std::map<std::string, std::string> headers_;
//     std::string body_;
//     size_t content_length_;

//     void parseHeaders(const std::string &headerLines);
//     int parseContentLength();

//     // Helper functions
//     int stringToInt(const std::string &str);
// };

// #endif

#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <string>
#include <map>

class HttpRequestParser {
private:
    std::string method_;
    std::string target_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    int content_length_;

    int stringToInt(const std::string &str);

public:
    HttpRequestParser();
    ~HttpRequestParser();

    int parseRequest(const std::string &request);
    void parseHeaders(const std::string &headerLines);
    int parseContentLength();

    std::string getMethod() const;
    std::string getTarget() const;
    std::string getProtocol() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;
    int getContentLength() const;
};

#endif
