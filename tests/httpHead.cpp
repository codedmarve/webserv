#include "../inc/Test.hpp"


void httpHead() {
    // Test different forms of HTTP HEAD requests
    std::vector<std::string> headRequests;
    headRequests.push_back("HEAD /resource/123 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    headRequests.push_back("HEAD /user/456 HTTP/1.1\r\nHost: api.example.com\r\nAuthorization: Bearer token123\r\n\r\n");
    headRequests.push_back("HEAD /file/image.jpg HTTP/1.1\r\nHost: files.example.com\r\nCookie: session_id=abc123; user_pref=dark_mode\r\n\r\n");
    headRequests.push_back("HEAD /post/789 HTTP/1.1\r\nHost: blog.example.com\r\nReferer: https://www.referer-example.com/previous-page\r\n\r\n");
    headRequests.push_back("HEAD /resource/123 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: MyCustomUserAgent\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nReferer: https://www.referer-example.com/previous-page\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nIf-Modified-Since: Sat, 26 Feb 2022 12:00:00 GMT\r\nCookie: session_id=abc123; user_pref=dark_mode\r\n\r\n");

    HttpRequestParser parser;
    // Process each HEAD request
    for (size_t i = 0; i < headRequests.size(); ++i) {
        parser.printRequest(headRequests[i]);
    }
}
