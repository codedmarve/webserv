#include "./inc/Test.hpp"

int main()
{
    // Invalid Scheme:
    //     ftp:/example.com
    //     ://example.com
    //     9https://example.com
    //     h&%ttps://example.com

    // Invalid Authority:
    //     https:/example.com
    //     https://example.com:port
    //     https://@example.com
    //     https://exam$ple.com

    // Invalid Path:
    //     https://example.com/invalid path
    //     https://example.com/!invalid_path
    //     https://example.com/invalid$path

    // Invalid Query:
    //     https://example.com/?invalid=query
    //     https://example.com/?invalid?query
    //     https://example.com/?invalid&query

    // Invalid Fragment:
    //     https://example.com/#invalid fragment
    //     https://example.com/#invalid$fragment
    //     https://example.com/#invalid@fragment

    HttpRequestParser parser;
    std::vector<std::string> testRequests;

    testRequests.push_back("GET https://www.example.com:8080/path/to/resource?param1=value1&param2=value2#section2 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    parser.parseRequest(testRequests[0]);

    // testRequests.push_back("GET https://www.example.com:8080/path/to/resource?param1=value1&param2=value2#section2 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET ftp:/example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET ://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET 9https://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET h&%ttps://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https:/example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com:port HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://@example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://exam$ple.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/invalid path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/!invalid_path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/invalid$path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/?invalid=query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/?invalid?query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/?invalid&query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/#invalid fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/#invalid$fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    // testRequests.push_back("GET https://example.com/#invalid@fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");

    for (size_t i = 0; i < testRequests.size(); ++i) {
        std::cout << "Validating URI: " << testRequests[i] << std::endl;
        if (parser.validateURI(testRequests[i])) {
            std::cout << "URI is valid!" << std::endl;
        } else {
            std::cout << "URI is NOT valid!" << std::endl;
        }
        std::cout << "END -> \n" << std::endl;
    }

    // httpGet();
    // httpPost();
    // httpPut();
    // httpDelete();
    // httpHead();
    // httpPatch();
    // httpOptions();
    // httpConnect();

    return (0);
}
