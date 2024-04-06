#include "../../inc/AllHeaders.hpp"

/// @todo 
// check path modifiers
// location

RequestConfig::RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db) {
    (void) client_;
    (void) host_port_;
}

RequestConfig::~RequestConfig() {}


VecStr RequestConfig::filterDataByDirectives(const std::vector<KeyMapValue>& values, std::string directive, std::string location = "") {
    VecStr result;
    modifierType_ = NONE;
    std::string locationExtract;

    for (size_t i = 0; i < values.size(); ++i) {
        const MapStr& keyMap = values[i].first;
        const VecStr& valueVector = values[i].second;
        
        MapStr::const_iterator directiveIt = keyMap.find("directives");
        MapStr::const_iterator locationIt = keyMap.find("location");

        if (directiveIt != keyMap.end() && directiveIt->second == directive) {
            if (locationIt != keyMap.end()) {


                locationExtract = checkModifier(locationIt->second);
                if (locationExtract == location) 
                    return valueVector;
            } 
        }
        modifierType_ = NONE;
    }

    return result;
}

std::string RequestConfig::checkModifier(const std::string& locationStr) {
    std::string modifiers;
    std::string newStr;
    size_t j;
    for (j = 0; j < locationStr.size(); ++j) {
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

    newStr = locationStr.substr(j);
    
    if (hasCaret && hasTilde)
        modifierType_ = LONGEST;
    else if (hasTilde && hasAsterisk)
        modifierType_ = CASE_INSENSITIVE;
    else if (hasEquals)
        modifierType_ = EXACT;
    else if (hasTilde)
        modifierType_ = CASE_SENSITIVE;
    return newStr;
}

void RequestConfig::setUp(size_t targetServerIdx) {
    targetServer_ = getDataByIdx(db_.serversDB, targetServerIdx);

    // printAllDBData(db_.serversDB);
    // printData(targetServer);

    /// @note
    // important to first pre-populate data inn cascades:
    // 1. preffered settings
    // 2. http level
    // 3. server level(locatn == "" == server-default settings)
    // 4. location level
    setTarget(request_.getURI());
    std::cout << "**********" << request_.getURI() << std::endl;

    VecStr result = filterDataByDirectives(targetServer_, "autoindex", target_);
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << "Value: " << result[i] << std::endl;
    }

}

void RequestConfig::setTarget(const std::string &target) {
    target_ = target;
}

std::string &RequestConfig::getTarget() {
  return target_;
}


std::string &RequestConfig::getRequestTarget() {

    return request_.getURI();
}

std::string &RequestConfig::getQuery() {
  return request_.getQuery();
}

std::string &RequestConfig::getHost() {
  return host_port_.ip_;
}

uint32_t &RequestConfig::getPort() {
  return host_port_.port_;
}