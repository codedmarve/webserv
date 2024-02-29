#include "../inc/Test.hpp"


void httpConnect() {
    // Test different forms of HTTP CONNECT requests
    std::vector<std::string> connectRequests;
    connectRequests.push_back("CONNECT www.example.com:443 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    connectRequests.push_back("CONNECT proxy.example.com:8080 HTTP/1.1\r\nHost: proxy.example.com\r\nAuthorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==\r\n\r\n");
    connectRequests.push_back("CONNECT www.example.com:443 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: MyCustomUserAgent\r\nAuthorization: Basic QWxhZGRpbjpvcGVuIHNlc2FtZQ==\r\n\r\n");

    HttpRequestParser parser;
    // Process each CONNECT request
    for (size_t i = 0; i < connectRequests.size(); ++i) {
        parser.printRequest(connectRequests[i]);
    }
}
