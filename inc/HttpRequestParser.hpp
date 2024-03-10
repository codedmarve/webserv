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
#include <set>

class HttpRequestParser {
private:
    std::string method_;
    std::string uri_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    bool extractComponents(const std::string& uri, std::string& scheme, std::string& authority, std::string& path, std::string& query, std::string& fragment);
    bool isValidScheme(const std::string& scheme);
    bool isValidAuthority(const std::string& authority);
    bool isValidPath(const std::string& path);
    bool isValidQuery(const std::string& query);
    bool isValidFragment(const std::string& fragment);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNum(char c);



    // struct IsValidChar {
    //     const std::string& validSet;
    //     IsValidChar(const std::string& set) : validSet(set) {}
    //     bool operator()(char c) const {
    //         return validSet.find(c) != std::string::npos;
    //     }
    // };


    // Helper struct to check if a character is in a given set of valid characters

    // bool isValidChar(char c, const std::string &validSet);

public:
    HttpRequestParser();

    int parseRequest(const std::string &request);
    void parseHeaders(const std::string &headerLines);
    void parseChunkedBody(const std::string &chunkedBody);
    void parseContentLength();
    int parseRequestLine();
    bool isMethodCharValid(char ch) const;
    int parseMethod();
    bool validateURI(const std::string &uri);
    int parseRequestLine(std::string headerLines, size_t eofFirstLine);

    std::string getMethod() const;
    std::string getURI() const;
    std::string getProtocol() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

    void printRequest(const std::string& request);

};

#endif
