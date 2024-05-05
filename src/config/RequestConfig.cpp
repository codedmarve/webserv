#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequest &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db)
{
}

RequestConfig::~RequestConfig()
{
    cgi_.clear();
    error_codes_.clear();
}

const VecStr &RequestConfig::filterDataByDirectives(const std::vector<KeyMapValue> &targetServ, std::string directive, std::string location = "")
{
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
                locationExtract = locationExtractor(locationIt->second);
                if (locationExtract == location)
                    return valueVector;
            }
        }
    }

    static VecStr emptyVector;
    return emptyVector;
}

bool RequestConfig::directiveExists(std::string directive, std::string location)
{
    for (size_t i = 0; i < targetServer_.size(); ++i)
    {
        const MapStr &keyMap = targetServer_[i].first;
        MapStr::const_iterator dirIt = keyMap.find("directives");
        MapStr::const_iterator locIt = keyMap.find("location");

        (void)directive;

        if (dirIt != keyMap.end() && locIt != keyMap.end())
        {
            std::string dir = dirIt->second;
            std::string loc = locIt->second;

            if (loc == location && dir == directive)
            {
                std::cout << "Checking: Directive = " << dir << ", Location = " << loc << std::endl;
                return true;
            }
        }
    }
    return false; // Directive not found in the specified location
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
std::string RequestConfig::locationExtractor(const std::string &locationStr)
{
    size_t j;

    for (j = 0; j < locationStr.size(); ++j)
    {
        char ch = locationStr[j];

        if (!(ch == '^' || ch == '*' || ch == '~' || ch == '=' || ch == '_'))
            break;
    }
    return locationStr.substr(j);
}

LocationModifier RequestConfig::checkModifier(const std::string &locationStr)
{
    LocationModifier modifierType_ = NONE;
    std::string modifiers;
    size_t pos = locationStr.find_first_of("/");
    if (pos != std::string::npos)
    {
        modifiers = locationStr.substr(0, pos);
    }
    else
    {
        return modifierType_;
    }

    std::set<char> validModifiers;
    validModifiers.insert('^');
    validModifiers.insert('*');
    validModifiers.insert('~');
    validModifiers.insert('=');
    validModifiers.insert('_');

    for (size_t j = 0; j < modifiers.size(); ++j)
    {
        char ch = modifiers[j];

        if (validModifiers.find(ch) == validModifiers.end())
            throw std::runtime_error("Invalid location modifier");
    }

    bool hasTilde = (modifiers.find("~") != std::string::npos);
    bool hasAsterisk = (modifiers.find("*") != std::string::npos);
    bool hasCaret = (modifiers.find("^") != std::string::npos);
    bool hasEquals = (modifiers.find("=") != std::string::npos);

    if (hasCaret && hasTilde)
        modifierType_ = LONGEST;
    else if (hasTilde && hasAsterisk)
        modifierType_ = CASE_INSENSITIVE;
    else if (hasEquals)
        modifierType_ = EXACT;
    else if (hasTilde)
        modifierType_ = CASE_SENSITIVE;

    return modifierType_;
}

/**
 * SETTERS
 */

RequestConfig *RequestConfig::getRequestLocation(std::string request_target)
{
    RequestConfig *requestConfig = NULL;
    // std::map<std::string,int regex_locations;
    std::map<std::string, int>::iterator it = locationsMap_.begin();
    while (it != locationsMap_.end())
    {
        // std::cout << "Modifiers: " << it->second << std::endl;
        // std::cout << "Uri: " << it->first << std::endl;
        // std::cout << "Target: " << request_target << std::endl;

        if (it->second != CASE_SENSITIVE && it->second != CASE_INSENSITIVE)
        {
            if (it->second == 1 && it->first == request_target)
                return &(*this);
            else if (request_target.find(it->first) == 0)
            {
                if (requestConfig && requestConfig->uri_.length() < it->first.length())
                    requestConfig = &(*this);
                else if (!requestConfig)
                    requestConfig = &(*this);
            }
        }
        // else
        // reg_locations.push_back(&(*it));

        it++;
    }
    if (requestConfig)
        std::cout << "RequestConfig: " << requestConfig->uri_ << std::endl;
    //   if (location && location->modifier_ == 4)
    // return location;

    // if (location && !location->locations_.empty())
    // {
    //     for (std::vector<ServerConfig>::iterator it = location->locations_.begin(); it != location->locations_.end(); it++)
    //     {
    //     if (it->modifier_ == 2 || it->modifier_ == 3)
    //         reg_locations.insert(reg_locations.begin(), &(*it));
    //     }
    // }
    (void)request_target;
    return requestConfig;
}

void RequestConfig::setLocationsMap(const std::vector<KeyMapValue> &values)
{
    int modifier = NONE;
    for (size_t i = 0; i < values.size(); ++i)
    {
        const MapStr &keyMap = values[i].first;
        std::string loc = keyMap.find("location")->second;
        modifier = checkModifier(loc);

        if (!loc.empty() && locationsMap_.find(loc) == locationsMap_.end())
        {
            locationsMap_[loc] = modifier;
        }
    }
}

void RequestConfig::setUp(size_t targetServerIdx)
{
    targetServer_ = getDataByIdx(db_.serversDB, targetServerIdx);
    serverId = targetServerIdx;

    setLocationsMap(targetServer_);
    setTarget(request_.getTarget());
    setUri(request_.getURI());
    setRoot(cascadeFilter("root", target_));
    setClientMaxBodySize(cascadeFilter("client_max_body_size", target_));
    setAutoIndex(cascadeFilter("autoindex", target_));
    setIndexes(cascadeFilter("index", target_));
    setErrorPages(cascadeFilter("error_page", target_));
    setMethods(cascadeFilter("allow_methods", target_));
    setAuth(cascadeFilter("auth", target_));
    setCgi(cascadeFilter("cgi", target_));
    setCgiBin(cascadeFilter("cgi-bin", target_));
    setRedirectMap(cascadeFilter("return", target_));

    printMap(getRedirectionMap());

    // RequestConfig *location = NULL;
    int status = request_.getStatus();
    if (status != 200 && status != 100)
    {
        // location = getRequestLocation(request_.getTarget());
    }
    std::cout << std::endl;
}

void RequestConfig::redirectLocation(std::string target)
{
    // RequestConfig *location = NULL;
    int status = request_.getStatus();
    if (status != 200 && status != 100)
    {
        // location = getRequestLocation(target);
    }
    target_ = target;
}

void RequestConfig::setTarget(const std::string &target)
{
    target_ = target;
}

void RequestConfig::setRoot(const VecStr root)
{

    root_ = root.empty() ? "html" : root[0];
}

void RequestConfig::setAuth(const VecStr &auth)
{
    auth_ = auth.empty() ? "off" : auth[0];
}

void RequestConfig::setUri(const std::string uri)
{
    uri_ = uri;
}

void RequestConfig::setClientMaxBodySize(const VecStr size)
{
    size_t val = size.empty() ? 20971520 : (std::istringstream(size[0]) >> val, val);
    client_max_body_size_ = val;
}

void RequestConfig::setUpload(const VecStr &upload)
{
    upload_ = upload[0];
}

void RequestConfig::setAutoIndex(const VecStr autoindex)
{
    autoindex_ = autoindex.empty() ? false : (autoindex[0] == "on");
}

void RequestConfig::setAutoIndex(bool autoindex)
{
    autoindex_ = autoindex;
}

void RequestConfig::setIndexes(const VecStr &indexes)
{
    indexes_ = indexes;
}

void RequestConfig::setMethods(const VecStr &methods)
{
    allowed_methods_ = methods.empty() ? cascadeFilter("limit_except", target_) : methods;
}

void RequestConfig::setCgi(const VecStr &cgi)
{
    cgi_.clear();

    if (cgi.size() % 2 != 0)
        std::cerr << "Warning: Cgi value is empty\n";

    for (size_t i = 0; i < cgi.size(); i += 2)
    {
        const std::string &key = cgi[i];
        std::string value;

        if (i + 1 < cgi.size())
            value = cgi[i + 1];
        cgi_[key] = value;
    }
}

void RequestConfig::setCgiBin(const VecStr &cgi)
{
    cgi_bin_ = (cgi_bin_.empty()) ? "" : cgi[0];
}

// void RequestConfig::setErrorPages(const VecStr &errors)
// {
//     std::map<int, std::string> resultMap;
//     std::string errorCodes;

//     if (!errors.empty())
//     {
//         for (size_t i = 0; i < errors.size(); ++i)
//         {
//             std::istringstream iss(errors[i]);
//             int errorCode;
//             if (iss >> errorCode)
//             {
//                 // Its errorCode. concatenate it
//                 if (!errorCodes.empty())
//                     errorCodes += " ";
//                 errorCodes += errors[i];
//             }
//             else
//             { // It's errorPage
//                 if (!errorCodes.empty())
//                 {
//                     // Split the concatenated errorCodes
//                     std::istringstream codeStream(errorCodes);
//                     int code;
//                     while (codeStream >> code)
//                     {
//                         resultMap[code] = errors[i];
//                     }
//                     errorCodes.clear();
//                 }
//             }
//         }

//         // Assign the last errorPage to remaining errorCodes
//         if (!errorCodes.empty())
//         {
//             std::istringstream codeStream(errorCodes);
//             int code;
//             while (codeStream >> code)
//             {
//                 resultMap[code] = errors.back();
//             }
//         }
//     }
//     error_codes_ = resultMap;
// }

void RequestConfig::assignErrorCodes(const std::string& errorCodes, const std::string& errorPage, std::map<int, std::string>& resultMap)
{
    std::istringstream codeStream(errorCodes);
    int code;
    while (codeStream >> code)
    {
        resultMap[code] = errorPage;
    }
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
            { 
                if (!errorCodes.empty())
                {
                    assignErrorCodes(errorCodes, errors[i], resultMap);
                    errorCodes.clear();
                }
            }
        }

        // Assign the last errorPage to remaining errorCodes
        if (!errorCodes.empty())
        {
            assignErrorCodes(errorCodes, errors.back(), resultMap);
        }
    }
    error_codes_ = resultMap;
}

void RequestConfig::assignRedirCodes(const std::string& errorCodes, const std::string& errorPage, std::map<int, std::string>& resultMap)
{
    std::istringstream codeStream(errorCodes);
    int code;
    while (codeStream >> code)
    {
        resultMap[code] = errorPage;
    }
}

void RequestConfig::setRedirectMap(const VecStr &redirectMap)
{
    std::map<int, std::string> resultMap;
    std::string redirCodes;

    if (!redirectMap.empty())
    {
        for (size_t i = 0; i < redirectMap.size(); ++i)
        {
            std::istringstream iss(redirectMap[i]);
            int redirectCode;
            if (iss >> redirectCode)
            {
                // Its errorCode. concatenate it
                if (!redirCodes.empty())
                    redirCodes += " ";
                redirCodes += redirectMap[i];
            }
            else
            { 
                if (!redirCodes.empty())
                {
                    assignRedirCodes(redirCodes, redirectMap[i], resultMap);
                    redirCodes.clear();
                }
            }
        }

        // Assign the last errorPage to remaining errorCodes
        if (!redirCodes.empty())
        {
            assignRedirCodes(redirCodes, redirectMap.back(), resultMap);
        }
    }
    redirectMap_ = resultMap;
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

size_t RequestConfig::getContentLength()
{
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

std::string &RequestConfig::getAuth()
{
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

std::map<int, std::string> &RequestConfig::getRedirectionMap()
{
    return redirectMap_;
}

std::vector<std::string> &RequestConfig::getMethods()
{
    return allowed_methods_;
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

std::map<std::string, std::string> &RequestConfig::getCgi()
{
    return cgi_;
}

std::string &RequestConfig::getCgiBin()
{
    return cgi_bin_;
}

std::map<std::string, int> &RequestConfig::getLocationsMap()
{
    return locationsMap_;
}

bool RequestConfig::isMethodAccepted(std::string &method)
{
    bool methodFlag = directiveExists("allow_methods", target_) || directiveExists("limit_except", target_);

    if (!methodFlag)
        return true;

    return (allowed_methods_[0] == "none" || method.empty()) ? false : (std::find(allowed_methods_.begin(), allowed_methods_.end(), method) != allowed_methods_.end());
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
    std::cout << "\nCGI\n";
    printMap(cgi_);
    std::cout << std::endl;
    std::cout << "\nCGI-BIN: " << getCgiBin() << std::endl;

    std::cout << "\n[Accepted Method] " << isMethodAccepted(getMethod());
    std::cout << "\n[content-length] " << getContentLength() << "\n"
              << std::endl;

    // printAllDBData(db_.serversDB);
    // printData(targetServer);
    // printVec(cascadeFilter("default_type", target_), "setup");
}
