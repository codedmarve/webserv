#include "../inc/Test.hpp"


// Callback function to receive HTTP response
#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>

size_t writeCallback(char* buf, size_t size, size_t nmemb, void* userdata) {
    ((std::string*)userdata)->append(buf, size * nmemb);
    return size * nmemb;
}

void testWithCurl() {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl." << std::endl;
        return;
    }

    std::vector<std::string> urls;
    urls.push_back("https://www.example.com");
    urls.push_back("https://www.google.com");

    for (size_t i = 0; i < urls.size(); ++i) {
        curl_easy_setopt(curl, CURLOPT_URL, urls[i].c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cout << "GET " << urls[i] << " - Response Code: " << http_code << std::endl;
            std::cout << "Response body:" << std::endl;
            std::cout << response << std::endl;
        }
    }

    curl_easy_cleanup(curl);
}

void httpGet() {
    std::string request = "GET /index.html HTTP/1.1\r\n"
                          "Host: www.example.com\r\n"
                          "Content-Length: 12\r\n"
                          "\r\n"
                          "Hello, World";

    HttpRequestParser parser;
    int result = parser.parseRequest(request);
    if (result == 0) {
        std::cout << "Method: " << parser.getMethod() << std::endl;
        std::cout << "Target: " << parser.getTarget() << std::endl;
        std::cout << "Protocol: " << parser.getProtocol() << std::endl;

        std::cout << "Headers:" << std::endl;
        std::map<std::string, std::string> headers = parser.getHeaders();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            std::cout << it->first << ": " << it->second << std::endl;
        }

        std::cout << "Body: " << parser.getBody() << std::endl;
        std::cout << "Content-Length: " << parser.getContentLength() << std::endl;
    } else {
        std::cout << "Parsing failed." << std::endl;
    }
}


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
        std::cout << "-----------------------------" << std::endl;
    }
}
