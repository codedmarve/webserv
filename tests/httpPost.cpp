
#include "../inc/Test.hpp"

void httpPost() {
    std::vector<std::string> postRequests;
    
    // Form URL Encoded Data
    std::string formUrlEncodedRequest =
        "POST /submit-form HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "username=johndoe&password=secret123";

    // JSON Data
    std::string jsonRequest =
        "POST /api/users HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/json\r\n"
        "\r\n"
        "{\n"
        "  \"name\": \"John Doe\",\n"
        "  \"email\": \"johndoe@example.com\",\n"
        "  \"age\": 30,\n"
        "  \"active\": true\n"
        "}";

    // Multipart Form Data
    std::string multipartFormDataRequest =
        "POST /upload-file HTTP/1.1\r\n"
        "Host: upload.example.com\r\n"
        "Content-Type: multipart/form-data; boundary=---------------------------1234567890\r\n"
        "\r\n"
        "-----------------------------1234567890\r\n"
        "Content-Disposition: form-data; name=\"file\"; filename=\"example.txt\"\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "(contents of the file)\r\n"
        "-----------------------------1234567890--";

    // XML Data
    std::string xmlRequest =
        "POST /api/xml-data HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/xml\r\n"
        "\r\n"
        "<user>\n"
        "  <name>John Doe</name>\n"
        "  <email>johndoe@example.com</email>\n"
        "  <age>30</age>\n"
        "  <active>true</active>\n"
        "</user>";

    // Binary Data (Image Upload)
    std::string binaryDataRequest =
        "POST /upload-image HTTP/1.1\r\n"
        "Host: images.example.com\r\n"
        "Content-Type: image/jpeg\r\n"
        "Content-Length: (size)\r\n"
        "\r\n"
        "(binary image data)";

    std::string chunkedBinaryDataRequest =
        "POST /upload-image HTTP/1.1\r\n"
        "Host: images.example.com\r\n"
        "Content-Type: image/jpeg\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "7\r\n"
        "chunked\r\n"
        "8\r\n"
        "encoding\r\n"
        "0\r\n"
        "\r\n";


    postRequests.push_back(formUrlEncodedRequest);
    postRequests.push_back(jsonRequest);
    postRequests.push_back(multipartFormDataRequest);
    postRequests.push_back(xmlRequest);
    postRequests.push_back(binaryDataRequest);
    postRequests.push_back(chunkedBinaryDataRequest);

    HttpRequestParser parser;

    for (size_t i = 0; i < postRequests.size(); ++i) {
        std::cout << "Testing HTTP POST Request " << i + 1 << ":\n";
        parser.printRequest(postRequests[i]);
    }
}
