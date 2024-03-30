#ifndef HTTP_REQUEST_PARSER_HPP
#define HTTP_REQUEST_PARSER_HPP

#include "./AllHeaders.hpp"

class HttpRequestParser {
private:

    std::string method_;
    std::string uri_;
    std::string protocol_;
    std::map<std::string, std::string> headers_;
    std::string body_;
    std::string scheme_;
    std::string authority_;
    std::string path_;
    std::string query_;
    std::string frag_;
    bool isChunked_;
    size_t length_;
    size_t chunk_size_;
    std::string req_buffer_;
    int body_offset_; // track current pos in req body and append next chunk there
    struct timeval start_tv_;
    struct timeval last_tv_;

    enum Section {
        REQUEST_LINE,
        HEADERS,
        SPECIAL_HEADERS,
        BODY,
        CHUNK,
        COMPLETE,
        ERROR
    };
    enum ChunkStatus {
        // CHUNK,
        CHUNK_BODY,
        CHUNK_SIZE
    };

    Section buffer_section_;
    ChunkStatus chunk_status_;

    int extractURIComponents();
    bool isValidScheme(const std::string& scheme);
    int isValidAuthority(const std::string& authority);
    bool isValidPath(const std::string& path);
    bool isValidQuery(const std::string& query);
    bool isValidFragment(const std::string& fragment);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNum(char c);
    void print_uri_extracts();
    bool isUnreserved(char c);
    bool isSubDelim(char c);
    bool isHexDigit(char c);
    bool isValidIPv6(const std::string& ipv6);
    int isValidProtocol(const std::string& protocol);
    std::string trim(const std::string& str);

    unsigned int hexToDecimal(const std::string& hex);
    void parseContentLength();
    bool isMethodCharValid(char ch) const;
    int parseMethod();
    int validateURI();
    int extractRequestLineData(std::string requestLine);
    std::string trimmer(const std::string& str);
    // void handleSpecialHeaders(const std::string& header, const std::string& value);
    bool isValidHeaderChar(unsigned char c);
    bool isValidHeaderFormat(const std::string& header);
    int parseHeaders();
    int parseRequestLine();
    int parseBody();
    int parseChunkTrailer();
    int parseChunkedBody();
    int parseChunkSize(const std::string& hex);
    int checkSpecialHeaders();
    void extractPathQueryFragment(size_t pathStart);
    void parseSchemeAndAuthority(size_t schemeEnd, size_t& pathStart);
    std::string decodeURIComponent(const std::string& str);


public:
    HttpRequestParser();
    ~HttpRequestParser();

    int parseRequest(std::string &buffer);

    std::string getMethod() const;
    std::string getURI() const;
    std::string getProtocol() const;
    std::string getBody() const;
    std::string &getQuery();
    std::string &getHeader(std::string key);
    std::map<std::string, std::string> getHeaders() const;

    void printRequest(HttpRequestParser parser);
};

#endif
