/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:15:28 by alappas           #+#    #+#             */
/*   Updated: 2024/05/29 21:41:13 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequest &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db)
{
    isLociMatched_ = 0;
    redir_code_ = 0;
    location_cache_ = "";
}

RequestConfig::~RequestConfig()
{
    redir_code_ = 0;
    cgi_.clear();
    error_codes_.clear();
}

RequestConfig::RequestConfig(const RequestConfig &rhs)
    : request_(rhs.request_), targetServer_(rhs.targetServer_), client_(rhs.client_), host_port_(rhs.host_port_), db_(rhs.db_),
      modifierType_(rhs.modifierType_), target_(rhs.target_), root_(rhs.root_), uri_(rhs.uri_),
      client_max_body_size_(rhs.client_max_body_size_), autoindex_(rhs.autoindex_),
      indexes_(rhs.indexes_), error_codes_(rhs.error_codes_), redirectMap_(rhs.redirectMap_),
      allowed_methods_(rhs.allowed_methods_), serverId(rhs.serverId), auth_(rhs.auth_),
      upload_(rhs.upload_), cgi_(rhs.cgi_), locationsMap_(rhs.locationsMap_),
      isLociMatched_(rhs.isLociMatched_), uri_suffix_(rhs.uri_suffix_), redir_code_(rhs.redir_code_)
{
}

RequestConfig::RequestConfig(const RequestConfig &rhs, HttpRequest &request, Client &client)
    : request_(request), targetServer_(rhs.targetServer_), client_(client), host_port_(rhs.host_port_), db_(rhs.db_),
      modifierType_(rhs.modifierType_), target_(rhs.target_), root_(rhs.root_), uri_(rhs.uri_),
      client_max_body_size_(rhs.client_max_body_size_), autoindex_(rhs.autoindex_),
      indexes_(rhs.indexes_), error_codes_(rhs.error_codes_), redirectMap_(rhs.redirectMap_),
      allowed_methods_(rhs.allowed_methods_), serverId(rhs.serverId), auth_(rhs.auth_),
      upload_(rhs.upload_), cgi_(rhs.cgi_), locationsMap_(rhs.locationsMap_),
      isLociMatched_(rhs.isLociMatched_), uri_suffix_(rhs.uri_suffix_), redir_code_(rhs.redir_code_)
{
}

RequestConfig &RequestConfig::operator=(const RequestConfig &rhs)
{
    if (this != &rhs)
    {
        request_ = rhs.request_;
        targetServer_ = rhs.targetServer_;
        client_ = rhs.client_;
        host_port_ = rhs.host_port_;
        modifierType_ = rhs.modifierType_;
        target_ = rhs.target_;
        root_ = rhs.root_;
        uri_ = rhs.uri_;
        client_max_body_size_ = rhs.client_max_body_size_;
        autoindex_ = rhs.autoindex_;
        indexes_ = rhs.indexes_;
        error_codes_ = rhs.error_codes_;
        redirectMap_ = rhs.redirectMap_;
        allowed_methods_ = rhs.allowed_methods_;
        serverId = rhs.serverId;
        auth_ = rhs.auth_;
        upload_ = rhs.upload_;
        cgi_ = rhs.cgi_;
        locationsMap_ = rhs.locationsMap_;
        isLociMatched_ = rhs.isLociMatched_;
        uri_suffix_ = rhs.uri_suffix_;
        redir_code_ = rhs.redir_code_;
    }
    return *this;
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
    if (location.empty())
        location = "/";

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
                return true;
        }
    }
    return false;
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

LocationModifier RequestConfig::setModifier(const std::string &locationStr)
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
    else if (hasTilde)
        modifierType_ = CASE_SENSITIVE;
    else if (hasEquals)
        modifierType_ = EXACT;

    return modifierType_;
}

/**
 * SETTERS
 */

RequestConfig *RequestConfig::getRequestLocation(std::string request_target)
{
    RequestConfig *requestConfig = NULL;

    std::map<std::string, int>::iterator it = locationsMap_.begin();
    while (it != locationsMap_.end())
    {
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
        it++;
    }

    return requestConfig;
}

int &RequestConfig::getRedirCode()
{
    return redir_code_;
}

void RequestConfig::setRedirCode(int code)
{
    redir_code_ = code;
}

void RequestConfig::setLocationsMap(const std::vector<KeyMapValue> &values)
{
    int modifier = NONE;
    for (size_t i = 0; i < values.size(); ++i)
    {
        const MapStr &keyMap = values[i].first;
        std::string loc = keyMap.find("location")->second;
        modifier = setModifier(loc);

        if (!loc.empty() && locationsMap_.find(loc) == locationsMap_.end())
        {
            locationsMap_[locationExtractor(loc)] = modifier;
        }
    }
}

void RequestConfig::returnRedirection()
{
    std::map<int, std::string> m = getRedirectionMap();
    if (getRedirectionMap().size())
    {
        std::map<int, std::string>::const_iterator it = m.begin();
        for (it = m.begin(); it != m.end(); ++it)
        {
            request_.setTarget(m[it->first]);
            setRedirCode(it->first);
        }
    }
}

std::pair<std::string, int> findCaseInsensitive(const std::map<std::string, int> &myMap, const std::string &key)
{
    std::string lowerKey;
    std::transform(key.begin(), key.end(), std::back_inserter(lowerKey), ::tolower);
    for (std::map<std::string, int>::const_iterator it = myMap.begin(); it != myMap.end(); ++it)
    {
        std::string lowerFirst;
        std::transform(it->first.begin(), it->first.end(), std::back_inserter(lowerFirst), ::tolower);
        if (lowerFirst == lowerKey)
            return *it;
    }
    return std::pair<std::string, int>("", 0);
}

std::string RequestConfig::findLongestMatch()
{
    std::string target = request_.getTarget();
	std::string longestMatch = "";

	std::map<std::string, int>::const_iterator locationsMap = getLocationsMap().begin();
	while (locationsMap != getLocationsMap().end())
	{
		if (target.find(locationsMap->first) == 0 && locationsMap->first.length() > longestMatch.length())
			longestMatch = locationsMap->first;
		locationsMap++;
	}
    return longestMatch;
}

std::string RequestConfig::findLongestMatch(std::string target)
{
    std::string longestMatch = "";

    std::map<std::string, int>::const_iterator locationsMap = getLocationsMap().begin();
    while (locationsMap != getLocationsMap().end())
    {
        if (target.find(locationsMap->first) == 0 && locationsMap->first.length() > longestMatch.length())
            longestMatch = locationsMap->first;
        locationsMap++;
    }
    return longestMatch;
}

void RequestConfig::setLociMatched(int status)
{
    isLociMatched_ = status;
}

int RequestConfig::getLociMatched()
{
    return isLociMatched_;
}

void RequestConfig::setBestMatch(std::string &newTarget)
{
    std::string target = request_.getTarget();
    std::string longestMatch = findLongestMatch();

    if (!longestMatch.empty())
    {
        newTarget = target.substr(0, longestMatch.length());
        target.erase(0, longestMatch.length());
        setTarget("/" + target);
        setUri("/" + target);
    }
    else
    {
        newTarget = request_.getTarget();
        setTarget(request_.getTarget());
        setUri(request_.getURI());
    }
}

void RequestConfig::setUp(size_t targetServerIdx)
{
    std::string newTarget;
    targetServer_ = getDataByIdx(db_.serversDB, targetServerIdx);
    serverId = targetServerIdx;

    setRedirectMap(cascadeFilter("return", request_.getTarget()));
    returnRedirection();
    setLocationsMap(targetServer_);
    setBestMatch(newTarget);
    setRoot(cascadeFilter("root", newTarget));
    setClientMaxBodySize(cascadeFilter("client_max_body_size", newTarget));
    setAutoIndex(cascadeFilter("autoindex", newTarget));
    setIndexes(cascadeFilter("index", newTarget));
    setErrorPages(cascadeFilter("error_page", newTarget));
    setMethods(cascadeFilter("allow_methods", newTarget));
    setAuth(cascadeFilter("auth", newTarget));
    setCgi(cascadeFilter("cgi", newTarget));
}

void RequestConfig::redirectLocation(std::string target)
{
    target_ = target;
}

void RequestConfig::setTarget(const std::string &target)
{

    target_ = removeDupSlashes(target);
}

void RequestConfig::setRoot(const VecStr root)
{

    root_ = root.empty() ? "./" : root[0];
}

void RequestConfig::setAuth(const VecStr &auth)
{
    auth_ = auth.empty() ? "off" : auth[0];
}

void RequestConfig::setUri(const std::string uri)
{
    uri_ = removeDupSlashes(uri);
}

void RequestConfig::setClientMaxBodySize(const VecStr size)
{
    size_t val = size.empty() ? 209715200 : (std::istringstream(size[0]) >> val, val);
    client_max_body_size_ = val;
}

void RequestConfig::setUpload(const VecStr &upload)
{
    upload_ = upload[0];
}

void RequestConfig::setCgi(bool& val)
{
    client_.setCgi(val);
}

bool RequestConfig::get_Cgi()
{
    return client_.getCgi();
}

bool RequestConfig::isCgi(std::string path)
{
     size_t lastDotPos = path.find_last_of(".");
    std::string ext = "";
    if (lastDotPos != std::string::npos && lastDotPos != 0)
        ext = path.substr(lastDotPos);
    bool result = std::find(cgi_.begin(), cgi_.end(), ext) != cgi_.end();
    client_.setCgi(result);
    return result;
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
    cgi_ = cgi;
}

void RequestConfig::assignCodes(const std::string &codes, const std::string &page, std::map<int, std::string> &resultMap)
{
    std::istringstream codeStream(codes);
    int code;
    while (codeStream >> code)
    {
        resultMap[code] = page;
    }
}

void RequestConfig::setMap(const VecStr &vec, std::map<int, std::string> &resultMap, std::string &codes)
{
    if (!vec.empty())
    {
        for (size_t i = 0; i < vec.size(); ++i)
        {
            std::istringstream iss(vec[i]);
            int code;
            if (iss >> code)
            {
                if (!codes.empty())
                    codes += " ";
                codes += vec[i];
            }
            else
            {
                if (!codes.empty())
                {
                    assignCodes(codes, vec[i], resultMap);
                    codes.clear();
                }
            }
        }

        if (!codes.empty())
            assignCodes(codes, vec.back(), resultMap);
    }
}

void RequestConfig::setErrorPages(const VecStr &errors)
{
    std::map<int, std::string> resultMap;
    std::string errorCodes;
    setMap(errors, resultMap, errorCodes);
    error_codes_ = resultMap;
}

void RequestConfig::setRedirectMap(const VecStr &redirectMap)
{
    std::map<int, std::string> resultMap;
    std::string redirCodes;
    setMap(redirectMap, resultMap, redirCodes);
    redirectMap_ = resultMap;
}

/**
 * GETTERS
 */

std::string RequestConfig::getUriSuffix()
{
    return request_.getUriSuffix();
}

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

const std::string &RequestConfig::getBody() const
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

std::vector<std::string> &RequestConfig::getCgi()
{
    return cgi_;
}

std::map<std::string, int> &RequestConfig::getLocationsMap()
{
    return locationsMap_;
}

bool RequestConfig::isMethodAccepted(std::string &method)
{
    bool methodFlag = false;
    location_cache_ = findLongestMatch(request_.getURI());
    setMethods(cascadeFilter("allow_methods", location_cache_));
    methodFlag = directiveExists("allow_methods", location_cache_) || directiveExists("limit_except", location_cache_);

    if (!methodFlag)
        return true;

    if (allowed_methods_.empty())
    {
        allowed_methods_.push_back("GET");
        allowed_methods_.push_back("POST");
        allowed_methods_.push_back("DELETE");
    }
    return (method.empty()) ? false : (std::find(allowed_methods_.begin(), allowed_methods_.end(), method) != allowed_methods_.end());
}

void RequestConfig::printConfigSetUp()
{
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
    printVec(cgi_, "SETUP");
    std::cout << std::endl;

    std::cout << "\n[Accepted Method] " << isMethodAccepted(getMethod());
    std::cout << "\n[content-length] " << getContentLength() << "\n"
              << std::endl;
}

void RequestConfig::setSubstr(int start)
{
    std::string body = request_.getBody();
	body = body.substr(start);
}

void RequestConfig::setClient(Client &client)
{
    client_ = client;
}

std::string &RequestConfig::getBody()
{
    return request_.getBody();
}