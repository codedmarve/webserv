#include "../../inc/AllHeaders.hpp"

/// @todo 
// check path modifiers
// location

RequestConfig::RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db) {
}

RequestConfig::~RequestConfig() {}


VecStr RequestConfig::filterDataByDirectives(const std::vector<KeyMapValue>& values, std::string directive, std::string location = "") {
    VecStr result;

    modifierType_ = NONE;
    for (size_t i = 0; i < values.size(); ++i) {
        const MapStr& keyMap = values[i].first;
        const VecStr& valueVector = values[i].second;
        MapStr::const_iterator it = keyMap.find("directives");
        MapStr::const_iterator it2 = keyMap.find("location");
        (void) location;

        if (it != keyMap.end() && it->second == directive) {
            if (it2 != keyMap.end() && it2->second == location) {
                return valueVector;
            } else if (it2 != keyMap.end() && it2->second.find(location) != std::string::npos) {
                modifierType_ = checkModifier(it2->second);
                std::cout << "ModifierType_: " << modifierType_ << std::endl;
                return (valueVector);
            }
        }
    }
    return result;
}

LocationModifier RequestConfig::checkModifier(const std::string& locationStr) {
    std::string modifiers;

    for (size_t j = 0; j < locationStr.size(); ++j) {
        char ch = locationStr[j];

        if (ch == '^' || ch == '*' || ch == '~' || ch == '=' || ch == '_') {
            if (ch != '_')
                modifiers += ch;
        } else
            break;
    }

    bool hasTilde = (modifiers.find("~") != std::string::npos);
    bool hasAsterisk = (modifiers.find("*") != std::string::npos);
    bool hasCaret = (modifiers.find("^") != std::string::npos);
    bool hasEquals = (modifiers.find("=") != std::string::npos);

    if (hasCaret && hasTilde)
        return LONGEST;
    else if (hasTilde && hasAsterisk)
        return CASE_INSENSITIVE;
    else if (hasEquals)
        return EXACT;
    else if (hasTilde)
        return CASE_SENSITIVE;
    return NONE;
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






