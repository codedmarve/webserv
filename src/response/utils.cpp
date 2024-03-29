#include "../../inc/AllHeaders.hpp"

std::string getMimeType(const std::string& ext) {
  std::map<std::string, std::string> mimeMap;
    // Common text formats
    mimeMap[".html"] = "text/html";
    mimeMap[".htm"] = "text/html";
    mimeMap[".css"] = "text/css";
    mimeMap[".js"] = "application/javascript";
    mimeMap[".json"] = "application/json";
    mimeMap[".xml"] = "application/xml";
    mimeMap[".txt"] = "text/plain";

    // Images
    mimeMap[".jpg"] = "image/jpeg";
    mimeMap[".jpeg"] = "image/jpeg";
    mimeMap[".png"] = "image/png";
    mimeMap[".gif"] = "image/gif";
    mimeMap[".bmp"] = "image/bmp";
    mimeMap[".ico"] = "image/x-icon";
    mimeMap[".svg"] = "image/svg+xml";

    // Audio and Video
    mimeMap[".mp3"] = "audio/mpeg";
    mimeMap[".mp4"] = "video/mp4";
    mimeMap[".mpeg"] = "video/mpeg";
    mimeMap[".ogg"] = "audio/ogg";
    mimeMap[".webm"] = "video/webm";
    mimeMap[".wav"] = "audio/wav";

    // Archives and Compressed Formats
    mimeMap[".zip"] = "application/zip";
    mimeMap[".rar"] = "application/x-rar-compressed";
    mimeMap[".tar"] = "application/x-tar";
    mimeMap[".gz"] = "application/gzip";
    mimeMap[".bz2"] = "application/x-bzip2";

    // Documents
    mimeMap[".pdf"] = "application/pdf";
    mimeMap[".doc"] = "application/msword";
    mimeMap[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeMap[".xls"] = "application/vnd.ms-excel";
    mimeMap[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeMap[".ppt"] = "application/vnd.ms-powerpoint";
    mimeMap[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";

    // Fonts
    mimeMap[".ttf"] = "font/ttf";
    mimeMap[".otf"] = "font/otf";
    mimeMap[".woff"] = "font/woff";
    mimeMap[".woff2"] = "font/woff2";

    // Others
    mimeMap[".avi"] = "video/x-msvideo";
    mimeMap[".bz"] = "application/x-bzip";
    mimeMap[".c"] = "text/x-c";
    mimeMap[".c11amc"] = "application/vnd.cluetrust.cartomobile-config";
    mimeMap[".c11amz"] = "application/vnd.cluetrust.cartomobile-config-pkg";
    mimeMap[".c4d"] = "application/vnd.clonk.c4group";
    mimeMap[".c4f"] = "application/vnd.clonk.c4group";
    mimeMap[".c4g"] = "application/vnd.clonk.c4group";
    mimeMap[".c4p"] = "application/vnd.clonk.c4group";
    mimeMap[".c4u"] = "application/vnd.clonk.c4group";
    mimeMap[".cab"] = "application/vnd.ms-cab-compressed";
    mimeMap[".caf"] = "audio/x-caf";
    mimeMap[".cap"] = "application/vnd.tcpdump.pcap";
    mimeMap[".car"] = "application/vnd.curl.car";
    mimeMap[".cat"] = "application/vnd.ms-pki.seccat";
    mimeMap[".cb7"] = "application/x-cbr";
    mimeMap[".cba"] = "application/x-cbr";
    mimeMap[".cbl"] = "application/vnd.cinderella";
    mimeMap[".cbr"] = "application/x-cbr";
    mimeMap[".cbt"] = "application/x-cbr";
    mimeMap[".cbz"] = "application/x-cbr";
    mimeMap[".cc"] = "text/x-c";
    mimeMap[".cct"] = "application/x-director";
    mimeMap[".ccxml"] = "application/ccxml+xml";
    mimeMap[".cdbcmsg"] = "application/vnd.contact.cmsg";
    mimeMap[".cdf"] = "application/x-netcdf";
    mimeMap[".cdkey"] = "application/vnd.mediastation.cdkey";
    mimeMap[".cdmia"] = "application/cdmi-capability";
    mimeMap[".cdmic"] = "application/cdmi-container";
    mimeMap[".cdmid"] = "application/cdmi-domain";
    mimeMap[".cdmio"] = "application/cdmi-object";
    mimeMap[".cdmiq"] = "application/cdmi-queue";
    mimeMap[".cdx"] = "chemical/x-cdx";
    mimeMap[".cdxml"] = "application/vnd.chemdraw+xml";
    mimeMap[".cdy"] = "application/vnd.cinderella";
    std::map<std::string, std::string>::iterator it = mimeMap.find(ext);
    return (it != mimeMap.end())
        ? it->second
        : "application/octet-stream";
}

std::string getHttpStatusCode(int code) {
    std::map<int, std::string> codeMap;

      // 1xx Informational
    codeMap[100] = "Continue";
    codeMap[101] = "Switching Protocols";
    codeMap[102] = "Processing";

    // 2xx Success
    codeMap[200] = "OK";
    codeMap[201] = "Created";
    codeMap[202] = "Accepted";
    codeMap[203] = "Non-Authoritative Information";
    codeMap[204] = "No Content";
    codeMap[205] = "Reset Content";
    codeMap[206] = "Partial Content";
    codeMap[207] = "Multi-Status";
    codeMap[208] = "Already Reported";
    codeMap[226] = "IM Used";

    // 3xx Redirection
    codeMap[300] = "Multiple Choices";
    codeMap[301] = "Moved Permanently";
    codeMap[302] = "Found";
    codeMap[303] = "See Other";
    codeMap[304] = "Not Modified";
    codeMap[305] = "Use Proxy";
    codeMap[307] = "Temporary Redirect";
    codeMap[308] = "Permanent Redirect";

    // 4xx Client Error
    codeMap[400] = "Bad Request";
    codeMap[401] = "Unauthorized";
    codeMap[402] = "Payment Required";
    codeMap[403] = "Forbidden";
    codeMap[404] = "Not Found";
    codeMap[405] = "Method Not Allowed";
    codeMap[406] = "Not Acceptable";
    codeMap[407] = "Proxy Authentication Required";
    codeMap[408] = "Request Timeout";
    codeMap[409] = "Conflict";
    codeMap[410] = "Gone";
    codeMap[411] = "Length Required";
    codeMap[412] = "Precondition Failed";
    codeMap[413] = "Payload Too Large";
    codeMap[414] = "URI Too Long";
    codeMap[415] = "Unsupported Media Type";
    codeMap[416] = "Range Not Satisfiable";
    codeMap[417] = "Expectation Failed";
    codeMap[418] = "I'm a teapot";
    codeMap[421] = "Misdirected Request";
    codeMap[422] = "Unprocessable Entity";
    codeMap[423] = "Locked";
    codeMap[424] = "Failed Dependency";
    codeMap[426] = "Upgrade Required";
    codeMap[428] = "Precondition Required";
    codeMap[429] = "Too Many Requests";
    codeMap[431] = "Request Header Fields Too Large";
    codeMap[451] = "Unavailable For Legal Reasons";

    // 5xx Server Error
    codeMap[500] = "Internal Server Error";
    codeMap[501] = "Not Implemented";
    codeMap[502] = "Bad Gateway";
    codeMap[503] = "Service Unavailable";
    codeMap[504] = "Gateway Timeout";
    codeMap[505] = "HTTP Version Not Supported";
    codeMap[506] = "Variant Also Negotiates";
    codeMap[507] = "Insufficient Storage";
    codeMap[508] = "Loop Detected";
    codeMap[510] = "Not Extended";
    codeMap[511] = "Network Authentication Required";

    std::map<int, std::string>::iterator it = codeMap.find(code);
    return (it != codeMap.end()) ? it->second : "Unknown";
}


std::string Utils::to_lower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string Utils::to_upper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string Utils::trim_left(const std::string& str, char c) {
    std::string result = str;
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [c](unsigned char ch) { return ch != c; }));
    return result;
}

std::string Utils::trim_right(const std::string& str, char c) {
    std::string result = str;
    result.erase(std::find_if(result.rbegin(), result.rend(), [c](unsigned char ch) { return ch != c; }).base(), result.end());
    return result;
}

std::string Utils::unique_char(const std::string& str) {
    std::string result = str;
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

int Utils::stoi(const std::string& str, std::size_t *pos, int base) {
    return std::stoi(str, pos, base);
}