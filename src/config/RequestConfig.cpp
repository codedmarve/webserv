#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequest &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db)
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
    setAuth(cascadeFilter("credentials", target_));

    printConfigSetUp();
}

void RequestConfig::printConfigSetUp()
{
    /// @note debugging purpose
    std::cout << "\nTarget: " << getTarget() << std::endl;
    std::cout << "\nURI: " << getUri() << std::endl;
    std::cout << "\nROOT: " << getRoot() << std::endl;
    std::cout << "\nHOST: " << getHost() << std::endl;
    std::cout << "\nPORT: " << getPort() << std::endl;
    std::cout << "\nMETHOD: " << getMethod() << std::endl;
    std::cout << "\nCLIENTMAXBODY: " << getClientMaxBodySize() << std::endl;
    std::cout << "\nAUTOINDEX: " << getAutoIndex() << std::endl;
    std::cout << getProtocol() << std::endl;
    std::cout << "\nBODY: " << getBody() << std::endl;
    std::cout << "\nINDEXES: \n";
    printVec(getIndexes(), "SETUP");
    std::cout << "\nERRORPAGES\n";
    printMap(getErrorPages());
    std::cout << "\nMETHODS\n";
    printVec(getMethods(), "SETUP");
    std::cout << "\nHEADERS\n";
    printMap(getHeaders());
    std::cout << std::endl;
    
    std::cout << "\n[Accepted Method] " << isMethodAccepted(getMethod());
    std::cout <<"\n[content-length] " << getContentLength() << "\n" << std::endl;

    // printAllDBData(db_.serversDB);
    // printData(targetServer);
    // printVec(cascadeFilter("default_type", target_), "setup");
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

void RequestConfig::setAuth(const VecStr &auth)
{
    if (auth.empty())
    {
        auth_ = "off";
        return;
    }
    auth_ = auth[0];
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

void RequestConfig::setUpload(const VecStr &upload)
{
    upload_ = upload[0];

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

    return headers;
}

std::string &RequestConfig::getTarget()
{
    return target_;
}

std::string &RequestConfig::getRequestTarget()
{
    return request_.getURI();
}

std::string &RequestConfig::getQuery()
{
    return request_.getQuery();
}

std::string &RequestConfig::getFragment()
{
    return request_.getFragment();
}

std::string &RequestConfig::getMethod()
{
    return request_.getMethod();
}

std::string &RequestConfig::getHost()
{
    return host_port_.ip_;
}

uint32_t &RequestConfig::getPort()
{
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
    return root_;
}

std::string &RequestConfig::getUri()
{
    return uri_;
}

std::string &RequestConfig::getAuth() {
  return auth_;
}

size_t &RequestConfig::getClientMaxBodySize()
{
    return client_max_body_size_;
}

bool RequestConfig::getAutoIndex()
{
    return autoindex_;
}

std::vector<std::string> &RequestConfig::getIndexes()
{
    return indexes_;
}

std::map<int, std::string> &RequestConfig::getErrorPages()
{
    return error_codes_;
}

std::vector<std::string> &RequestConfig::getMethods()
{
    return methods_;
}

std::string &RequestConfig::getBody()
{
    return request_.getBody();
}

std::string &RequestConfig::getHeader(std::string key)
{
    std::transform(key.begin(), key.end(), key.begin(), tolower);
    return request_.getHeader(key);
}

std::string &RequestConfig::getProtocol()
{
    return request_.getProtocol();
}

std::string &RequestConfig::getUpload()
{
    return upload_;
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


