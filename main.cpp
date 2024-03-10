#include "./inc/Test.hpp"

int main()
{
    HttpRequestParser parser;
    int x = parser.parseRequest("GET /hello HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
    std::cout << x << std::endl;
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
