#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db)
{
}

RequestConfig::~RequestConfig() {}

const VecStr &RequestConfig::filterDataByDirectives(const std::vector<KeyMapValue> &targetServ, std::string directive, std::string location = "")
{
    modifierType_ = NONE;
    std::string locationExtract;

    for (size_t i = 0; i < targetServ.size(); ++i)
    {
        const MapStr &keyMap = targetServ[i].first;
        const VecStr &valueVector = targetServ[i].second;

        MapStr::const_iterator directiveIt = keyMap.find("directives");
        if (directiveIt != keyMap.end() && directiveIt->second == directive)
        {
            MapStr::const_iterator locationIt = keyMap.find("location");
            if (locationIt != keyMap.end())
            {
                locationExtract = checkModifier(locationIt->second);
                if (locationExtract == location)
                    return valueVector;
            }
        }
        modifierType_ = NONE;
    }

    static VecStr emptyVector;
    return emptyVector;
}

const VecStr &RequestConfig::checkRootDB(std::string directive)
{
    GroupedDBMap::const_iterator it;
    for (it = db_.rootDB.begin(); it != db_.rootDB.end(); ++it)
    {
        const std::vector<ConfigDB::KeyMapValue> &values = it->second;
        for (size_t i = 0; i < values.size(); ++i)
        {
            const MapStr &keyMap = values[i].first;
            const VecStr &valueVector = values[i].second;
            std::string location = keyMap.find("location")->second;
            const VecStr &dirValue = filterDataByDirectives(values, directive, location);
            if (!dirValue.empty())
                return valueVector;
        }
    }

    static VecStr emptyVector;
    return emptyVector;
}

const VecStr &RequestConfig::cascadeFilter(std::string directive, std::string location = "")
{
    /// @note important to first pre-populate data in cascades:
    // 1. preffered settings
    // 2. http level
    // 3. server level(locatn == "" == server-default settings)
    // 4. location level

    const VecStr &dirValue = filterDataByDirectives(targetServer_, directive, location);
    if (!dirValue.empty())
        return dirValue;

    if (dirValue.empty() && !location.empty())
    {
        const VecStr &filteredValue = filterDataByDirectives(targetServer_, directive, "");
        if (!filteredValue.empty())
            return filteredValue;
    }

    return checkRootDB(directive);
}

std::string RequestConfig::checkModifier(const std::string &locationStr)
{
    std::string modifiers;
    std::string newStr;
    size_t j;
    for (j = 0; j < locationStr.size(); ++j)
    {
        char ch = locationStr[j];

        if (ch == '^' || ch == '*' || ch == '~' || ch == '=' || ch == '_')
        {
            if (ch != '_')
                modifiers += ch;
        }
        else
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

/**
 * SETTERS
 */
void RequestConfig::setUp(size_t targetServerIdx)
{
    targetServer_ = getDataByIdx(db_.serversDB, targetServerIdx);
    serverId = targetServerIdx;

    setTarget(request_.getURI());
    setUri(request_.getURI());
    setRoot(cascadeFilter("root", target_));
    setClientMaxBodySize(cascadeFilter("client_max_body_size", target_));
    setAutoIndex(cascadeFilter("autoindex", target_));
    setIndexes(cascadeFilter("index", target_));
    setErrorPages(cascadeFilter("error_page", target_));
    setMethods(cascadeFilter("allow_methods", target_));

    printConfigSetUp();
}

void RequestConfig::printConfigSetUp()
{
    /// @note debugging purpose
    getTarget();
    std::cout << std::endl;
    getUri();
    std::cout << std::endl;
    getRoot();
    std::cout << std::endl;
    getHost();
    std::cout << std::endl;
    getPort();
    std::cout << std::endl;
    getMethod();
    std::cout << std::endl;
    getClientMaxBodySize();
    std::cout << std::endl;
    getAutoIndex();
    std::cout << std::endl;
    getIndexes();
    std::cout << std::endl;
    getErrorPages();
    std::cout << std::endl;
    getMethods();
    std::cout << std::endl;
    getBody();
    std::cout << std::endl;
    getHeaders();
    std::cout << std::endl;
    getHeader("Host");
    std::cout << std::endl;
    getProtocol();
    std::cout << std::endl;
    std::cout << "[Accepted Method] " << isMethodAccepted(getMethod());
    std::cout <<"[content-length] " << getContentLength() << std::endl;
    ;
    // printAllDBData(db_.serversDB);
    // printData(targetServer);
    // VecStr result = filterDataByDirectives(targetServer_, "autoindex", target_);
    // VecStr result = cascadeFilter("default_type", target_);
    // for (size_t i = 0; i < result.size(); ++i)
    // {
    //     std::cout << "Value: " << result[i] << std::endl;
    // }
}

void RequestConfig::setTarget(const std::string &target)
{
    target_ = target;
}

void RequestConfig::setRoot(const VecStr root)
{
    if (root.empty())
    {
        root_ = "html";
        return;
    }
    root_ = root[0];
}

void RequestConfig::setUri(const std::string uri)
{
    uri_ = uri;
}

void RequestConfig::setClientMaxBodySize(const VecStr size)
{
    size_t val;
    if (size.empty())
    {
        client_max_body_size_ = 20971520; // default 20mb
        return;
    }
    std::istringstream iss(size[0]);
    iss >> val;
    client_max_body_size_ = val;
}

void RequestConfig::setAutoIndex(const VecStr autoindex)
{
    if (autoindex.empty())
    {
        autoindex_ = false;
        return;
    }
    autoindex_ = (autoindex[0] == "on") ? true : false;
}

void RequestConfig::setIndexes(const VecStr &indexes)
{
    indexes_ = indexes;
}

void RequestConfig::setMethods(const VecStr &methods)
{
    if (methods.empty())
    {
        methods_ = cascadeFilter("limit_except", target_);
        return;
    }

    methods_ = methods;
}

void RequestConfig::setErrorPages(const VecStr &errors)
{
    std::map<int, std::string> resultMap;
    std::string errorCodes;

    if (!errors.empty())
    {
        for (size_t i = 0; i < errors.size(); ++i)
        {
            std::istringstream iss(errors[i]);
            int errorCode;
            if (iss >> errorCode)
            {
                // Its errorCode. concatenate it
                if (!errorCodes.empty())
                    errorCodes += " ";
                errorCodes += errors[i];
            }
            else
            { // It's errorPage
                if (!errorCodes.empty())
                {
                    // Split the concatenated errorCodes
                    std::istringstream codeStream(errorCodes);
                    int code;
                    while (codeStream >> code)
                    {
                        resultMap[code] = errors[i];
                    }
                    errorCodes.clear();
                }
            }
        }

        // Assign the last errorPage to remaining errorCodes
        if (!errorCodes.empty())
        {
            std::istringstream codeStream(errorCodes);
            int code;
            while (codeStream >> code)
            {
                resultMap[code] = errors.back();
            }
        }
    }
    error_codes_ = resultMap;
}

/**
 * GETTERS
 */

std::map<std::string, std::string> RequestConfig::getHeaders()
{
    std::map<std::string, std::string>::const_iterator it;
    MapStr headers = request_.getHeaders();

    std::cout << "[Headers] \n";
    for (it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    return headers;
}

std::string &RequestConfig::getTarget()
{
    std::cout << "[target] " << target_ << "\n";
    return target_;
}

std::string &RequestConfig::getRequestTarget()
{
    return request_.getURI();
}

std::string &RequestConfig::getQuery()
{
    std::cout << "[query] " << request_.getQuery() << "\n";
    return request_.getQuery();
}

std::string &RequestConfig::getFragment()
{
    std::cout << "[fragment] " << request_.getFragment() << "\n";
    return request_.getFragment();
}

std::string &RequestConfig::getMethod()
{
    std::cout << "[Method] " << request_.getMethod() << "\n";
    return request_.getMethod();
}

std::string &RequestConfig::getHost()
{
    std::cout << "[ip] " << host_port_.ip_ << "\n";
    return host_port_.ip_;
}

uint32_t &RequestConfig::getPort()
{
    std::cout << "[port] " << host_port_.port_ << "\n";
    return host_port_.port_;
}

size_t RequestConfig::getContentLength() {
    return request_.getContentLength();

}

Client &RequestConfig::getClient()
{
    return client_;
}

std::string &RequestConfig::getRoot()
{
    std::cout << "[root] " << root_ << "\n";
    return root_;
}

std::string &RequestConfig::getUri()
{
    std::cout << "[uri] " << uri_ << "\n";
    return uri_;
}

size_t &RequestConfig::getClientMaxBodySize()
{
    std::cout << "[client_max_body_size] " << client_max_body_size_ << "\n";
    return client_max_body_size_;
}

bool RequestConfig::getAutoIndex()
{
    std::cout << "[autoindex] " << autoindex_ << "\n";
    return autoindex_;
}

std::vector<std::string> &RequestConfig::getIndexes()
{
    std::cout << "[index] ";
    for (size_t i = 0; i < indexes_.size(); ++i)
    {
        std::cout << indexes_[i] << " ";
    }
    std::cout << std::endl;
    return indexes_;
}

std::map<int, std::string> &RequestConfig::getErrorPages()
{
    std::map<int, std::string>::const_iterator it;

    std::cout << "[ErrorCodes/Pages] \n";
    for (it = error_codes_.begin(); it != error_codes_.end(); ++it)
    {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    return error_codes_;
}

std::vector<std::string> &RequestConfig::getMethods()
{
    std::map<int, std::string>::const_iterator it;

    std::cout << "[Methods] \n";
    for (size_t i = 0; i < methods_.size(); ++i)
    {
        std::cout << methods_[i] << " ";
    }
    std::cout << std::endl;
    return methods_;
}

std::string &RequestConfig::getBody()
{
    std::cout << "[Body]\n"
              << request_.getBody() << "\n";
    return request_.getBody();
}

std::string &RequestConfig::getHeader(std::string key)
{
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    std::cout << "[header] " << key << ": " << request_.getHeader(key) << "\n";
    return request_.getHeader(key);
}

std::string &RequestConfig::getProtocol()
{
    std::cout << "[protocol] " << request_.getProtocol() << "\n";
    return request_.getProtocol();
}

bool RequestConfig::isMethodAccepted(std::string &method)
{
    if (methods_.empty() || method.empty())
        return true;
    if (std::find(methods_.begin(), methods_.end(), method) != methods_.end())
        return true;
    return false;
}

void RequestConfig::redirectLocation(std::string target)
{
    target_ = target;
}
