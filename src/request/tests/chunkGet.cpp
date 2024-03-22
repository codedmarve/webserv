#include <iostream>
#include <fstream>
#include <string>
#include "../../../inc/Test.hpp"

int chunkedGet() {
    std::ifstream infile("get_data.txt");
    std::string line;
    std::string buffer;
    HttpRequestParser parser;

    bool result = false;
    if (!infile.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }
    
    while (std::getline(infile, line)) {
            std::cout << "HERE\n";
        buffer += line + "\r\n";
        result = parser.parseRequest(buffer);
    }

    parser.printRequest(parser);


    if (result) {
        std::cout << " Request parsed successfully." << std::endl;
    } else {
        std::cout << " Failed to parse request." << std::endl;
    }

    return 0;
}
