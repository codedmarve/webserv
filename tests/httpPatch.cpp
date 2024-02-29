#include "../inc/Test.hpp"

void httpPatch() {
    // Test different forms of HTTP PATCH requests
    std::vector<std::string> patchRequests;
    patchRequests.push_back("PATCH /resource/123 HTTP/1.1\r\nHost: www.example.com\r\nContent-Type: application/json\r\nContent-Length: 15\r\n\r\n{\"key\": \"value\"}");
    patchRequests.push_back("PATCH /user/456 HTTP/1.1\r\nHost: api.example.com\r\nAuthorization: Bearer token123\r\nContent-Type: application/json\r\nContent-Length: 20\r\n\r\n{\"key2\": \"value2\"}");
    patchRequests.push_back("PATCH /file/image.jpg HTTP/1.1\r\nHost: files.example.com\r\nCookie: session_id=abc123; user_pref=dark_mode\r\nContent-Type: application/json\r\nContent-Length: 25\r\n\r\n{\"key3\": \"value3\"}");
    patchRequests.push_back("PATCH /resource/123 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: MyCustomUserAgent\r\nAuthorization: Bearer token123\r\nContent-Type: application/json\r\nContent-Length: 25\r\n\r\n{\"id\": \"123\", \"update\": \"new data\"}");

    HttpRequestParser parser;
    // Process each PATCH request
    for (size_t i = 0; i < patchRequests.size(); ++i) {
        parser.printRequest(patchRequests[i]);
    }
}
