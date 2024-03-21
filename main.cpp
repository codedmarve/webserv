#include "./inc/Test.hpp"
// #include "./inc/HttpRequestParser.hpp"

// int main()
// {
//     // Invalid Scheme:
//     //     ftp:/example.com
//     //     ://example.com
//     //     9https://example.com
//     //     h&%ttps://example.com

//     // Invalid Authority:
//     //     https:/example.com
//     //     https://example.com:port
//     //     https://@example.com
//     //     https://exam$ple.com

//     // Invalid Path:
//     //     https://example.com/invalid path
//     //     https://example.com/!invalid_path
//     //     https://example.com/invalid$path

//     // Invalid Query:
//     //     https://example.com/?invalid=query
//     //     https://example.com/?invalid?query
//     //     https://example.com/?invalid&query

//     // Invalid Fragment:
//     //     https://example.com/#invalid fragment
//     //     https://example.com/#invalid$fragment
//     //     https://example.com/#invalid@fragment

//     HttpRequestParser parser;
//     std::vector<std::string> testRequests;

//     testRequests.push_back("POST /api/users HTTP/1.1\r\nContent-Type: application/json\r\nContent-Length: 42\r\n\r\n{\"name\": \"John Doe\",\"email\": \"johndoe@example.com\",\"age\": 30,\"active\": true}");
//     // parser.parseRequest(testRequests[0]);

//     // testRequests.push_back("GET https://example.com:8080/path/to/resource?param1=value1&param2=value2#section2 HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET ftp:/example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET ://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET 9https://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET h&%ttps://example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https:/example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com:port HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://@example.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://exam$ple.com HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/invalid path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/!invalid_path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/invalid$path HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/?invalid=query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/?invalid?query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/?invalid&query HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/#invalid fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/#invalid$fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");
//     // testRequests.push_back("GET https://example.com/#invalid@fragment HTTP/1.1\r\nHost: www.example.com\r\n\r\n");

//     for (size_t i = 0; i < testRequests.size(); ++i) {
//         // std::cout << "Validating URI: " << testRequests[i] << std::endl;
//         (parser.parseRequest(testRequests[i]))
//             ? std::cout << "URI is valid!\n" << std::endl
//             : std::cout << "URI is NOT valid!\n" << std::endl;
//         // std::cout << "END -> \n" << std::endl;
//     }
//     const std::map<std::string, std::string>& headers = parser.getHeaders();
//     std::map<std::string, std::string>::const_iterator it;
//     for (it = headers.begin(); it != headers.end(); ++it) {
//         std::cout << it->first << ": " << it->second << std::endl;
//     }

//     // httpGet();
//     // httpPost();
//     // httpPut();
//     // httpDelete();
//     // httpHead();
//     // httpPatch();
//     // httpOptions();
//     // httpConnect();

//     return (0);
// }

// #include <iostream>
// #include <fstream>
// #include <string>
// int main() {
//     std::ifstream infile("test_data.txt");
//     std::string line;
//     std::string buffer;
//     HttpRequestParser parser;

//     bool result = false; 
    
//     while (std::getline(infile, line)) {
//         buffer += line + "\r\n";
//         result = parser.parseRequest(buffer);
//     }

//     parser.printRequest(parser);


//     if (result) {
//         std::cout << " Request parsed successfully." << std::endl;
//     } else {
//         std::cout << " Failed to parse request." << std::endl;
//     }

//     return 0;
// }


// #include <iostream>
// #include <fstream>
// #include "HttpRequestParser.h" // Include your header file

int main() {
    // chunkedGet();

    chunkedPost();
    return 0;
}

