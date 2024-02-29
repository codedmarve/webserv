#include "../inc/Test.hpp"

void httpPut() {
    std::vector<std::string> putRequests;

    // Form URL Encoded Data
    std::string formUrlEncodedRequest =
        "PUT /update-data HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "field1=value1&field2=value2";
    putRequests.push_back(formUrlEncodedRequest);

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
    putRequests.push_back(jsonRequest);

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
    putRequests.push_back(xmlRequest);

    HttpRequestParser parser;

    for (size_t i = 0; i < putRequests.size(); ++i) {
        std::cout << "Testing HTTP PUT Request " << i + 1 << ":\n";
        parser.printRequest(putRequests[i]);
    }
}
