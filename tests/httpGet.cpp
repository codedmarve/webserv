#include "../inc/Test.hpp"


// Callback function to receive HTTP response
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

void testMultipleGet() {
    // Test different forms of HTTP GET requests
    std::vector<std::string> requests;
    requests.push_back("GET /hello HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    requests.push_back("GET /search?query=example&page=1 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    requests.push_back("GET /search?q=hello%20world&lang=en HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    requests.push_back("GET /resource HTTP/1.1\r\nHost: www.example.com\r\nAuthorization: Bearer token123\r\nUser-Agent: MyCustomUserAgent\r\n\r\n");
    requests.push_back("GET /users/1234/profile HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    requests.push_back("GET /dashboard HTTP/1.1\r\nHost: www.example.com\r\nCookie: session_id=abc123; user_pref=dark_mode\r\n\r\n");
    requests.push_back("GET /api/data HTTP/1.1\r\nHost: www.example.com\r\nCache-Control: no-cache\r\n\r\n");
    requests.push_back("GET /largefile.zip HTTP/1.1\r\nHost: www.example.com\r\nRange: bytes=0-1023\r\n\r\n");
    requests.push_back("GET /latest-data HTTP/1.1\r\nHost: www.example.com\r\nIf-Modified-Since: Sat, 26 Feb 2022 12:00:00 GMT\r\n\r\n");
    requests.push_back("GET /api/data HTTP/1.1\r\nHost: www.example.com\r\nAccept: application/json\r\n\r\n");
    requests.push_back("GET /target-page HTTP/1.1\r\nHost: www.example.com\r\nReferer: https://www.referer-example.com/previous-page\r\n\r\n");

    // Create an instance of HttpRequestParser
    HttpRequestParser parser;

    // Process each request
    for (size_t i = 0; i < requests.size(); ++i) {
        std::cout << "Parsing request:\n" << requests[i] << std::endl;
        int result = parser.parseRequest(requests[i]);
        std::cout << "Parsing result: " << result << std::endl;

        // Print Headers using iterators
        std::cout << "Headers:" << std::endl;
        std::map<std::string, std::string> headers = parser.getHeaders();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }

        std::cout << "-----------------------------" << std::endl;
    }
}


