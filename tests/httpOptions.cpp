#include "../inc/Test.hpp"


void httpOptions() {
    // Test different forms of HTTP OPTIONS requests
    std::vector<std::string> optionsRequests;
    optionsRequests.push_back("OPTIONS /resource/123 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    optionsRequests.push_back("OPTIONS /user/456 HTTP/1.1\r\nHost: api.example.com\r\nAuthorization: Bearer token123\r\n\r\n");
    optionsRequests.push_back("OPTIONS /file/image.jpg HTTP/1.1\r\nHost: files.example.com\r\nCookie: session_id=abc123; user_pref=dark_mode\r\n\r\n");
   optionsRequests.push_back("OPTIONS /resource/123 HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: MyCustomUserAgent\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nReferer: https://www.referer-example.com/previous-page\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\n\r\n");
 
    HttpRequestParser parser;
    // Process each OPTIONS request
    for (size_t i = 0; i < optionsRequests.size(); ++i) {
        parser.printRequest(optionsRequests[i]);
    }
}
