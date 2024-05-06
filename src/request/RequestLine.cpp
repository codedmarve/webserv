#include "../../inc/HttpRequest.hpp"

int HttpRequest::extractRequestLineData(std::string requestLine)
{
    std::istringstream iss(requestLine);
    size_t methodEnd;
    size_t uriStart;
    size_t protocolStart;

    methodEnd = requestLine.find(' ');
    uriStart = methodEnd + 1;
    protocolStart = requestLine.rfind(' ');

    // Check if both URI and protocol positions are valid
    if (methodEnd == std::string::npos || protocolStart == std::string::npos || protocolStart <= uriStart)
        return 400;

    // Extract METHOD, URI and Protocol
    method_ = requestLine.substr(0, methodEnd);
    uri_ = requestLine.substr(uriStart, protocolStart - uriStart);
    (uri_.find("?") != std::string::npos) ? setTarget(uri_.substr(0, uri_.find("?"))) : setTarget(uri_);
    protocol_ = requestLine.substr(protocolStart + 1);
    return 200;
}

int HttpRequest::parseRequestLine()
{

    size_t endOfFirstLine = req_buffer_.find("\r\n");
    if (endOfFirstLine == std::string::npos)
        return 400;

    std::string requestLine = req_buffer_.substr(0, endOfFirstLine);

    int httpStatus = extractRequestLineData(requestLine);
    if (httpStatus != 200)
        return httpStatus;

    httpStatus = parseMethod();
    if (httpStatus != 200)
        return httpStatus;

    httpStatus = validateURI();
    if (httpStatus != 200)
        return httpStatus;

    httpStatus = isValidProtocol(protocol_);
    if (httpStatus != 200)
        return httpStatus;

    buffer_section_ = HEADERS;
    req_buffer_.erase(0, endOfFirstLine + 2);

    return 200;
}

int HttpRequest::parseMethod()
{
    static const std::string validMethodsArray[] = {
        "GET", "POST", "PUT", "DELETE", "PATCH", "OPTIONS", "HEAD", "TRACE", "CONNECT"};
    static const std::set<std::string> validMethods(validMethodsArray, validMethodsArray + sizeof(validMethodsArray) / sizeof(validMethodsArray[0]));

    // Check if the method is valid and in uppercase
    if (validMethods.find(method_) == validMethods.end())
        return 501;

    for (std::string::const_iterator it = method_.begin(); it != method_.end(); ++it)
    {
        if (!isMethodCharValid(*it))
            return 400;
    }
    return 200;
}

void HttpRequest::parseSchemeAndAuthority(size_t schemeEnd, size_t &pathStart)
{
    size_t authorityStart;
    size_t authorityEnd;

    scheme_ = uri_.substr(0, schemeEnd);
    if (schemeEnd + 3 < uri_.length() && uri_.substr(schemeEnd, 3) == "://")
    {
        authorityStart = schemeEnd + 3;
        authorityEnd = uri_.find_first_of("/?#", authorityStart);
        if (authorityEnd == std::string::npos)
            authorityEnd = uri_.length();

        authority_ = uri_.substr(authorityStart, authorityEnd - authorityStart);
        pathStart = authorityEnd;
    }
}

void HttpRequest::extractPathQueryFragment(size_t pathStart)
{
    std::string decodedUri = decodeURIComponent(uri_);

    size_t queryStart = decodedUri.find('?', pathStart);
    size_t fragmentStart = decodedUri.find('#', pathStart);

    if (queryStart != std::string::npos)
    {
        if (fragmentStart != std::string::npos && fragmentStart > queryStart)
        {
            path_ = decodedUri.substr(pathStart, queryStart - pathStart);
            query_ = decodedUri.substr(queryStart + 1, fragmentStart - (queryStart + 1));
            frag_ = decodedUri.substr(fragmentStart + 1);
        }
        else
        {
            path_ = decodedUri.substr(pathStart, queryStart - pathStart);
            query_ = decodedUri.substr(queryStart + 1);
        }
    }
    else if (fragmentStart != std::string::npos)
    {
        path_ = decodedUri.substr(pathStart, fragmentStart - pathStart);
        frag_ = decodedUri.substr(fragmentStart + 1);
    }
    else
    {
        path_ = decodedUri.substr(pathStart);
    }

    path_ = decodeURIComponent(path_);
    query_ = decodeURIComponent(query_);
    frag_ = decodeURIComponent(frag_);
}

std::string HttpRequest::decodeURIComponent(const std::string &str)
{
    std::string decoded;
    for (size_t i = 0; i < str.length(); ++i)
    {
        if (str[i] == '%')
        { // Percent-encoded sequence
            if (i + 2 < str.length())
            {
                // Check if there are two hex characters following '%'
                char hex1 = str[i + 1];
                char hex2 = str[i + 2];
                if (isxdigit(hex1) && isxdigit(hex2))
                {
                    int value;
                    std::istringstream hexStream(str.substr(i + 1, 2));
                    hexStream >> std::hex >> value;      // Convert hex characters to integer value
                    decoded += static_cast<char>(value); // Append decoded character
                    i += 2;                              // Move to next character
                }
                else
                    decoded += str[i];
            }
            else
                decoded += str[i];
        }
        else if (str[i] == '+')
            decoded += ' ';
        else
            decoded += str[i];
    }
    return decoded;
}

int HttpRequest::extractURIComponents()
{
    size_t schemeEnd;
    size_t pathStart;

    pathStart = 0;
    schemeEnd = uri_.find(':');
    if (schemeEnd != std::string::npos)
    {
        parseSchemeAndAuthority(schemeEnd, pathStart);
    }

    extractPathQueryFragment(pathStart);

    return (path_.empty() && uri_ != "/") ? 400 : 200;
}

bool HttpRequest::isValidScheme(const std::string &scheme)
{
    if (scheme.empty())
        return true;

    if (!isAlpha(scheme[0]))
        return false; // Scheme must start with a letter

    std::string lowercaseScheme = scheme;
    for (size_t i = 0; i < lowercaseScheme.length(); ++i)
    {
        lowercaseScheme[i] = std::tolower(lowercaseScheme[i]);
    }

    const std::string validSchemeChars = "abcdefghijklmnopqrstuvwxyz0123456789+-.!$&'()*+,;=";
    for (size_t i = 0; i < lowercaseScheme.length(); ++i)
    {
        char ch = lowercaseScheme[i];
        if (!isAlphaNum(ch) && validSchemeChars.find(ch) == std::string::npos)
        {
            return false;
        }
    }

    return true;
}

int HttpRequest::isValidAuthority(const std::string &authority)
{
    if (authority.empty())
        return 400;
    size_t userinfoEnd = authority.find('@');
    std::string userinfo = userinfoEnd != std::string::npos ? authority.substr(0, userinfoEnd) : "";

    size_t hostStart = userinfoEnd != std::string::npos ? userinfoEnd + 1 : 0;
    size_t portStart = authority.find(':', hostStart);
    std::string host = portStart != std::string::npos ? authority.substr(hostStart, portStart - hostStart) : authority.substr(hostStart);

    std::string port = (portStart != std::string::npos && portStart + 1 < authority.length()) ? authority.substr(portStart + 1) : "";

    const std::string validAuthorityChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:";
    for (size_t i = 0; i < userinfo.length(); ++i)
    {
        if (!isUnreserved(userinfo[i]) && !isSubDelim(userinfo[i]) && userinfo[i] != ':')
        {
            return 401;
        }
    }

    // Check host, allowing for IPv6 and percent-encoded characters
    if (host.empty())
    {
        return 402; // Host cannot be empty
    }

    if (host[0] == '[' && host[host.length() - 1] == ']')
    {
        // IPv6 address format, remove brackets for validation
        std::string ipv6Host = host.substr(1, host.length() - 2);
        if (!isValidIPv6(ipv6Host))
        {
            return 402;
        }
    }
    else
    {
        // Regular hostname or IPv4 address
        for (size_t i = 0; i < host.length(); ++i)
        {
            if (!isUnreserved(host[i]) && !isSubDelim(host[i]) && host[i] != ':')
            {
                return 402;
            }
        }
    }

    // Check port
    if (!port.empty())
    {
        for (size_t i = 0; i < port.length(); ++i)
        {
            if (!isDigit(port[i]))
            {
                return 403;
            }
        }
        int portValue = atoi(port.c_str());
        if (portValue < 0 || portValue > 65535)
        {
            return 403; // Port out of valid range
        }
    }

    return 200;
}

bool HttpRequest::isValidPath(const std::string &path)
{
    const std::string validPathChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";

    if (path.empty() || path[0] != '/')
    {
        return false; // Path must start with '/'
    }

    for (size_t i = 0; i < path.length(); ++i)
    {
        char c = path[i];
        if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@' || c == '/')
        {
            continue; // Allowed characters
        }
        else if (c == '%' && i + 2 < path.length())
        {
            // Check percent-encoded characters
            char hex1 = path[i + 1];
            char hex2 = path[i + 2];
            if (isHexDigit(hex1) && isHexDigit(hex2))
            {
                i += 2;
                continue;
            }
        }
        return false;
    }

    return true;
}

bool HttpRequest::isValidQuery(const std::string &query)
{
    if (query.empty())
        return true;
    const std::string validQueryChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";

    for (size_t i = 0; i < query.length(); ++i)
    {
        char c = query[i];
        if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@' || c == '?' || c == '/' || c == '#' || c == '[' || c == ']' || c == '&' || c == '=' || c == '+' || c == '$' || c == ',' || c == ';')
        {
            continue; // Allowed characters
        }
        else if (c == '%' && i + 2 < query.length())
        {
            // Check percent-encoded characters
            char hex1 = query[i + 1];
            char hex2 = query[i + 2];
            if (isHexDigit(hex1) && isHexDigit(hex2))
            {
                i += 2;
                continue;
            }
        }
        return false;
    }

    return true;
}

bool HttpRequest::isValidFragment(const std::string &fragment)
{
    if (fragment.empty())
        return true;
    const std::string validFragmentChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=";

    for (size_t i = 0; i < fragment.length(); ++i)
    {
        char c = fragment[i];
        if (isUnreserved(c) || isSubDelim(c) || c == ':' || c == '@' || c == '?' || c == '/' || c == '#' || c == '[' || c == ']' || c == '&' || c == '=' || c == '+' || c == '$' || c == ',' || c == ';')
        {
            continue; // Allowed characters
        }
        else if (c == '%' && i + 2 < fragment.length())
        {
            // Check percent-encoded characters
            char hex1 = fragment[i + 1];
            char hex2 = fragment[i + 2];
            if (isHexDigit(hex1) && isHexDigit(hex2))
            {
                i += 2;
                continue;
            }
        }
        return false;
    }

    return true;
}

int HttpRequest::validateURI()
{
    if (uri_.empty())
        return false;
    int hasAuthority = extractURIComponents();
    // print_uri_extracts();

    if (!isValidScheme(scheme_))
        return 400;
    if (hasAuthority == 200 && !isValidAuthority(authority_))
        return 400;
    if (!isValidPath(path_))
        return 404;
    if (!isValidQuery(query_))
        return 405;
    if (!isValidFragment(frag_))
        return 406;
    return 200;
}

bool HttpRequest::isValidIPv6(const std::string &ipv6)
{
    // Count the number of colons
    size_t numColons = 0;
    for (size_t i = 0; i < ipv6.length(); ++i)
    {
        if (ipv6[i] == ':')
        {
            numColons++;
        }
    }

    // If there are more than 7 colons or it contains "::" without being the only "::",
    // it's not a valid IPv6 address
    if (numColons > 7 || ipv6.find("::") != std::string::npos || (numColons == 7 && ipv6[ipv6.length() - 1] == ':'))
    {
        return false;
    }

    // Split the string into groups separated by ":"
    std::vector<std::string> groups;
    std::string group;
    std::istringstream iss(ipv6);
    while (std::getline(iss, group, ':'))
    {
        groups.push_back(group);
    }

    // Check each group
    for (size_t i = 0; i < groups.size(); ++i)
    {
        const std::string &g = groups[i];
        // Each group should be 1-4 hexadecimal characters
        if (g.empty() || g.length() > 4)
        {
            return false;
        }

        // Check if all characters are hexadecimal
        for (size_t j = 0; j < g.length(); ++j)
        {
            char c = g[j];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
            {
                return false;
            }
        }
    }

    return true;
}

int HttpRequest::isValidProtocol(const std::string &protocol)
{
    // Format should be "HTTP/x.y"
    if (protocol.substr(0, 5) != "HTTP/")
        return false;

    // Check version format
    std::string version = protocol.substr(5);
    if (version.size() < 3 || version[0] != '1' || version[1] != '.')
        return 505; // Not Supported

    // Check if the rest of the version are digits
    for (size_t i = 2; i < version.size(); i++)
    {
        if (!isdigit(version[i]))
            return 505;
    }

    return 200;
}
