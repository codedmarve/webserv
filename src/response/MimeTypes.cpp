// #include "../../inc/AllHeaders.hpp"
#include "../../inc/MimeTypes.hpp"

MimeTypes::MimeTypes() {
    // Initialize the mapping of file extensions to MIME types
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
    mimeMap[".mpeg"] = "video/mpeg"/*  */;
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
}

std::string MimeTypes::getType(const std::string& extension) {
    std::map<std::string, std::string>::iterator it = mimeMap.find(extension);
    return (it != mimeMap.end())
        ? it->second
        : "application/octet-stream";
}

MimeTypes::~MimeTypes() {
    mimeMap.clear();
}