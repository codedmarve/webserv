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
        if (directiveIt != keyMap.end() && directiveIt->second == directive) {
            MapStr::const_iterator locationIt = keyMap.find("location");
            if (locationIt != keyMap.end()) {
                locationExtract = checkModifier(locationIt->second);
                if (locationExtract == location) {
                    std::cout << "DEBUGGER: " << locationExtract << "   " << location << std::endl;

                    return valueVector;
                }
            } 
        }
        modifierType_ = NONE;
    }

    return result;
}

VecStr RequestConfig::cascadeFilter(const std::vector<KeyMapValue>& values, std::string directive, std::string location = "") {
    VecStr dirValue;
    dirValue = filterDataByDirectives(values, directive, location);
        std::cout << location << "HERE\n";
    if (!dirValue.empty()) {
        return dirValue;
    }
    if (dirValue.empty() && !location.empty())
    {
        dirValue = filterDataByDirectives(values, directive, "");
    }
    if (dirValue.empty())
    {
        GroupedDBMap::const_iterator it;
        for (it = db_.rootDB.begin(); it != db_.rootDB.end(); ++it) {
            const std::vector<ConfigDB::KeyMapValue>& values = it->second;
            for (size_t i = 0; i < values.size(); ++i) {
                const MapStr& keyMap = values[i].first;
                const VecStr& valueVector = values[i].second;
                std::string location = keyMap.find("location")->second;
                    std::cout << "{ " 
                        << keyMap.find("directives")->second
                        << ", "  << keyMap.find("location")->second
                        << " }" << "\n";

                dirValue = filterDataByDirectives(values, directive, location);
                if (!dirValue.empty())
                    return valueVector;
            }
        }
    }
    return dirValue;
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
    setUri(request_.getURI());
    setRoot(cascadeFilter(targetServer_, "root", target_)[0]);
    setRoot(cascadeFilter(targetServer_, "client_max_body_size", target_)[0]);
    VecStr result = cascadeFilter(targetServer_, "default_type", target_);

    // VecStr result = filterDataByDirectives(targetServer_, "autoindex", target_);
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

Client &RequestConfig::getClient() {
  return client_;
}

void RequestConfig::setRoot(const std::string root) {
    root_ = root;
}

std::string &RequestConfig::getRoot() {
    std::cout << "root: " << root_ << "\n";
  return root_;
}

void RequestConfig::setUri(const std::string uri) {
  uri_ = uri;
}

std::string &RequestConfig::getUri() {
    std::cout << "uri: " << uri_ << "\n";
    return uri_;
}

void RequestConfig::setClientMaxBodySize(std::string size) {
    size_t val;
    std::istringstream iss(size);
    iss >> val;
    client_max_body_size_ = val;
}

size_t &RequestConfig::getClientMaxBodySize() {
  return client_max_body_size_;
}
