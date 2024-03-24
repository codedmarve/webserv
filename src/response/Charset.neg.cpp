#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

// Assume these are defined somewhere
#include "file.h"   // File class definition
#include "config.h" // Config class definition

// Mutex for thread safety
pthread_mutex_t g_write = PTHREAD_MUTEX_INITIALIZER;

class Response {
public:
  std::string accept_charset(std::vector<std::string> &matches) {
    std::string acceptCharset = config_.getHeader("Accept-Charset");
    std::vector<std::string> requestCharsets = parseAcceptCharsetHeader(acceptCharset);

    // Default charset is empty
    std::string selectedCharset = "";

    float maxQValue = 0.0;

    // Iterate over each requested charset
    for (const std::string &requestCharset : requestCharsets) {
      float qValue = getQValue(requestCharset);
      std::string baseCharset = getBaseCharset(requestCharset);

      // Find the best matching charset
      for (const std::string &availableCharset : matches) {
        if (matchCharset(baseCharset, availableCharset) && qValue > maxQValue) {
          selectedCharset = availableCharset;
          maxQValue = qValue;
        }
      }
    }

    // Return the selected charset
    return selectedCharset;
  }

private:
  std::map<std::string, std::string> headers_;
  std::string body_;
  std::string response_;

  // Assume these are defined somewhere
  File file_;
  Config config_;

  std::vector<std::string> parseAcceptCharsetHeader(const std::string &acceptCharset) {
    std::vector<std::string> charsets;
    std::istringstream ss(acceptCharset);
    std::string token;
    while (std::getline(ss, token, ',')) {
      charsets.push_back(token);
    }
    return charsets;
  }

  float getQValue(const std::string &charsetTag) {
    size_t pos = charsetTag.find(";q=");
    if (pos != std::string::npos) {
      try {
        return std::stof(charsetTag.substr(pos + 3));
      } catch (const std::exception &e) {
        return 1.0; // Default to 1.0 if parsing fails
      }
    }
    return 1.0; // Default q-value is 1.0 if not specified
  }

  std::string getBaseCharset(const std::string &charsetTag) {
    size_t pos = charsetTag.find(';');
    if (pos != std::string::npos) {
      return charsetTag.substr(0, pos);
    }
    return charsetTag;
  }

  bool matchCharset(const std::string &baseCharset, const std::string &availableCharset) {
    // Exact match
    if (baseCharset == availableCharset) {
      return true;
    }

    // Match with wildcard
    if (baseCharset == "*") {
      return true;
    }

    return false;
  }
};
