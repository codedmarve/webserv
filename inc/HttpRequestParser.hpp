#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <set>
#include <cstdlib>

class HttpRequestParser {
private:
    std::string method_;
    std::string uri_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;

    bool extractURIComponents(const std::string& uri, std::string& scheme, std::string& authority, std::string& path, std::string& query, std::string& fragment);
    bool isValidScheme(const std::string& scheme);
    bool isValidAuthority(const std::string& authority);
    bool isValidPath(const std::string& path);
    bool isValidQuery(const std::string& query);
    bool isValidFragment(const std::string& fragment);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNum(char c);
    void print_uri_extracts(const std::string& uri, std::string& scheme, std::string& authority, std::string& path, std::string& query, std::string& fragment);
    bool isUnreserved(char c);
    bool isSubDelim(char c);
    bool isHexDigit(char c);
    bool isValidIPv6(const std::string& ipv6);
    int isValidProtocol(const std::string& protocol);



public:
    HttpRequestParser();

    int parseRequest(const std::string &request);
    void parseHeaders(const std::string &headerLines);
    void parseChunkedBody(const std::string &chunkedBody);
    void parseContentLength();
    int parseRequestLine(std::string requestLine);
    bool isMethodCharValid(char ch) const;
    int parseMethod();
    int validateURI(const std::string &uri);
    int parseRequestLine(std::string headerLines, size_t eofFirstLine);
    int extractRequestLineData(std::string requestLine);

    std::string getMethod() const;
    std::string getURI() const;
    std::string getProtocol() const;
    std::map<std::string, std::string> getHeaders() const;
    std::string getBody() const;

    void printRequest(const std::string& request);

};

#endif
