#include "../../inc/AllHeaders.hpp"

/// @todo 
// check path modifiers
// location

RequestConfig::RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db) {
}

RequestConfig::~RequestConfig() {}


VecStr filterDataByDirectives(const std::vector<KeyMapValue>& values, std::string directive, std::string location = "") {
    VecStr result;

    for (size_t i = 0; i < values.size(); ++i) {
        const MapStr& keyMap = values[i].first;
        const VecStr& valueVector = values[i].second;
        MapStr::const_iterator it = keyMap.find("directives");
        MapStr::const_iterator it2 = keyMap.find("location");
        (void) location;

        if (it != keyMap.end() && it->second == directive
            && it2 != keyMap.end() && it2->second == location)
            return valueVector;
    }
    return result;
}

void RequestConfig::setUp() {
    std::vector<KeyMapValue> targetServer = getDataByIdx(db_.serversDB, 0);
    std::cout << "**********" << request_.getURI() << std::endl;

    // printAllDBData(db_.serversDB);
    // printData(targetServer);

    /// @note
    // important to first pre-populate data inn cascades:
    // 1. preffered settings
    // 2. http level
    // 3. server level(locatn == "" == server-default settings)
    // 4. location level

    VecStr result = filterDataByDirectives(targetServer, "autoindex", request_.getQuery());
    
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << "Value: " << result[i] << std::endl;
    }
}

// bool checkModifier(std::string &str) {
//     size_t tilde = str.find("~");
//     size_t asterisk = str.find("*");
//     size_t caret = str.find("^");
//     size_t eq = str.find("=");
//     return (str == "="
//         || str == "~"
//         || str == "~*"
//         || str == "~^"
//     );
// }



