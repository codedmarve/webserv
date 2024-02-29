#include "../inc/Test.hpp"

void httpDelete() {
    // Test different forms of HTTP DELETE requests
    std::vector<std::string> deleteRequests;
    deleteRequests.push_back("DELETE /resource/123 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    deleteRequests.push_back("DELETE /user/456 HTTP/1.1\r\nHost: api.example.com\r\nAuthorization: Bearer token123\r\n\r\n");
    deleteRequests.push_back("DELETE /file/image.jpg HTTP/1.1\r\nHost: files.example.com\r\nCookie: session_id=abc123; user_pref=dark_mode\r\n\r\n");
    deleteRequests.push_back("DELETE /post/789 HTTP/1.1\r\nHost: blog.example.com\r\nReferer: https://www.referer-example.com/previous-page\r\n\r\n");
    deleteRequests.push_back("DELETE /delete-resource HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/json\r\nContent-Length: 25\r\n\r\n{\"id\": \"123\", \"reason\": \"obsolete\"}");
    deleteRequests.push_back("DELETE /resource/123 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: MyCustomUserAgent\r\nAuthorization: Bearer token123\r\nContent-Type: application/json\r\nContent-Length: 25\r\n\r\n{\"id\": \"123\", \"reason\": \"obsolete\"}");



    HttpRequestParser parser;
    // Process each DELETE request
    for (size_t i = 0; i < deleteRequests.size(); ++i) {
        parser.printRequest(deleteRequests[i]);
    }
}
