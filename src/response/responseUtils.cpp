#include "../../inc/AllHeaders.hpp"

std::map<char, int> initializeB64Index() {
    std::map<char, int> index;
    const char B64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (int i = 0; i < 64; ++i) {
        index[B64chars[i]] = i;
    }
    return index;
}

// Initialize the B64 index map
const std::map<char, int> B64index = initializeB64Index();

// Base64 decode function
const std::string b64decode(const void *data, const size_t &len) {
    if (len == 0) return "";

    const unsigned char *p = static_cast<const unsigned char*>(data);
    size_t j = 0,
           pad1 = len % 4 || p[len - 1] == '=',
           pad2 = pad1 && (len % 4 > 2 || p[len - 2] != '=');
    const size_t last = (len - pad1) / 4 << 2;
    std::string result((last / 4 * 3) + pad1 + pad2, '\0');
    unsigned char *str = reinterpret_cast<unsigned char*>(&result[0]);

    for (size_t i = 0; i < last; i += 4) {
        int n = B64index.at(p[i]) << 18 | B64index.at(p[i + 1]) << 12 | B64index.at(p[i + 2]) << 6 | B64index.at(p[i + 3]);
        str[j++] = n >> 16;
        str[j++] = (n >> 8) & 0xFF;
        str[j++] = n & 0xFF;
    }
    if (pad1) {
        int n = B64index.at(p[last]) << 18 | B64index.at(p[last + 1]) << 12;
        str[j++] = n >> 16;
        if (pad2) {
            n |= B64index.at(p[last + 2]) << 6;
            str[j++] = (n >> 8) & 0xFF;
        }
    }
    return result;
}

// Base64 decode function overload for std::string input
std::string b64decode(const std::string &str64) {
    return b64decode(str64.c_str(), str64.size());
}


std::string ftos(size_t num) {
    std::ostringstream oss;
    oss << num;
    return oss.str();
}


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


template<typename T>
void printVec(const std::vector<T> &value, const std::string &callingFunction) {
    typename std::vector<T>::const_iterator it;
    std::cout << "Printing vector from " << callingFunction << std::endl;

    for (it = value.begin(); it != value.end(); ++it) {
        std::cout << "Value: " << *it << std::endl;
    }
    std::cout << std::endl;
}
template void printVec<std::string>(const std::vector<std::string> &value, const std::string &callingFunction);
template void printVec<int>(const std::vector<int> &value, const std::string &callingFunction);

template<typename KeyType, typename ValueType>
void printMap(const std::map<KeyType, ValueType> &m) {
    typename std::map<KeyType, ValueType>::const_iterator it;
    for (it = m.begin(); it != m.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    std::cout << std::endl;
}
template void printMap(const std::map<std::string, std::string> &m);
template void printMap(const std::map<int, std::string> &m);