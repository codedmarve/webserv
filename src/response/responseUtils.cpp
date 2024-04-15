#include "../../inc/AllHeaders.hpp"

const std::string b64decode(const void *data, const size_t &len) {
    if (len == 0) return "";

    // Base64 index for decoding
    const char B64index[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char *p = (unsigned char*)data;
    size_t j = 0,
           pad1 = len % 4 || p[len - 1] == '=',
           pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
    const size_t last = (len - pad1) / 4 << 2;
    std::string result(last / 4 * 3 + pad1 + pad2, '\0');
    unsigned char *str = (unsigned char*) &result[0];

    for (size_t i = 0; i < last; i += 4) {
        int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
        str[j++] = n >> 16;
        str[j++] = n >> 8 & 0xFF;
        str[j++] = n & 0xFF;
    }
    if (pad1) {
        int n = B64index[p[last]] << 18 | B64index[p[last + 1]] << 12;
        str[j++] = n >> 16;
        if (pad2) {
            n |= B64index[p[last + 2]] << 6;
            str[j++] = n >> 8 & 0xFF;
        }
    }
    return result;
}

std::string b64decode(const std::string &str64) {
    return b64decode(str64.c_str(), str64.size());
}


std::string ftos(size_t num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}

// std::string removeDupSlashes(std::string str)
// {
//     if (str.empty())
//         return str;

//     for (std::string::iterator it = str.begin(); it != str.end();) {
//         if (*it == '/') {
//             it++;
//             while (it != str.end() && *it == '/') {
//                 it = str.erase(it);
//             }
//         } else {
//             it++;
//         }
//     }
//     return str;
// }

std::string removeDupSlashes(std::string str) {
    if (str.empty())
        return str;

    std::string result;
    result.reserve(str.length());

    bool prevWasSlash = false;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '/') {
            if (!prevWasSlash) {
                result.push_back(str[i]);
            }
            prevWasSlash = true;
        } else {
            result.push_back(str[i]);
            prevWasSlash = false;
        }
    }

    // Remove the last slash if it exists
    if (!result.empty() && result[result.length() - 1] == '/') {
        result.erase(result.length() - 1);
    }

    return result;
}

std::string formatHttpDate(time_t timeValue) {
    char buf[32];
    struct tm* timeinfo = gmtime(&timeValue);

    // Adjust for CEST => should be +2 hours but for some weird reason +4 works
    timeinfo->tm_hour += 4;
    mktime(timeinfo);

    strftime(buf, sizeof(buf), "%a, %d %b %Y %T GMT", timeinfo);
    return std::string(buf);
}

std::string get_http_date() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t currentTime = tv.tv_sec;
    return formatHttpDate(currentTime);
}


std::string generateETagForFile(File& file) {
    std::stringstream ss;
    
    // Check file exists && a regular file
    if (file.is_file()) {
        std::string lastModified = file.last_modified();
        
        if (lastModified.empty() || lastModified == "Unknown")
            return "";
        
        // Convert lastModified to a number
        std::stringstream convert(lastModified);
        time_t modifiedTime;
        convert >> modifiedTime;

        // Convert modifiedTime to hexadecimal
        ss << std::hex << modifiedTime;
        
        return ss.str();
    } else 
    return "";
}

void printVecStr(std::vector<std::string> &value, std::string callingFunction) {
    std::vector<std::string>::iterator it;
    std::cout << "Printing vector from " << callingFunction << std::endl;

    for (it = value.begin(); it != value.end(); ++it) {
        std::cout << "Value: " << *it << std::endl;
    }
    std::cout << std::endl;
}
