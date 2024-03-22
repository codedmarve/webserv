#include <iostream>
#include <fstream>
#include "../../../inc/Test.hpp"

int chunkedPost() {
    std::ifstream infile("./post_data.txt");
    std::string line;
    std::string buffer;
    HttpRequestParser parser;

    bool result = false;
    if (!infile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    while (std::getline(infile, line)) {
        buffer += line + "\r\n";
        result = parser.parseRequest(buffer);
    }
    parser.printRequest(parser);
    if (result) {
        std::cout << "All requests parsed successfully." << std::endl;
    } else {
        std::cout << "Failed to parse one or more requests." << std::endl;
    }

    return 0;
}
