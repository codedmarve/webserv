#include "../inc/Test.hpp"

void testHttpPutRequests() {
    // Form URL Encoded Data
    std::string formUrlEncodedRequest =
        "PUT /update-data HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "field1=value1&field2=value2";

    // JSON Data
    std::string jsonRequest =
        "PUT /api/resource/123 HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\n"
        "  \"key1\": \"value1\",\n"
        "  \"key2\": \"value2\"\n"
        "}";

    // XML Data
    std::string xmlRequest =
        "PUT /api/update-info HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/xml\r\n"
        "\r\n"
        "<info>\n"
        "  <key1>value1</key1>\n"
        "  <key2>value2</key2>\n"
        "</info>";

    
    HttpRequestParser parser;

    std::cout << "Testing Form URL Encoded Data:\n";
    parser.printRequest(formUrlEncodedRequest);

    std::cout << "Testing JSON Data:\n";
    parser.printRequest(jsonRequest);

    std::cout << "Testing XML Data:\n";
    parser.printRequest(xmlRequest);
}
