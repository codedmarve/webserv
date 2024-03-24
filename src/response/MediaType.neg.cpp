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
  std::string media_type_negotiation(std::vector<std::string> &matches) {
    std::string acceptMediaType = config_.getHeader("Accept");
    std::vector<std::string> requestMediaTypes = parseAcceptMediaTypeHeader(acceptMediaType);

    // Default media type is empty
    std::string selectedMediaType = "";

    float maxQValue = 0.0;

    // Iterate over each requested media type
    for (const std::string &requestMediaType : requestMediaTypes) {
      float qValue = getQValue(requestMediaType);
      std::string baseMediaType = getBaseMediaType(requestMediaType);

      // Find the best matching media type
      for (const std::string &availableMediaType : matches) {
        if (matchMediaType(baseMediaType, availableMediaType) && qValue > maxQValue) {
          selectedMediaType = availableMediaType;
          maxQValue = qValue;
        }
      }
    }

    // Return the selected media type
    return selectedMediaType;
  }

private:
  std::map<std::string, std::string> headers_;
  std::string body_;
  std::string response_;

  // Assume these are defined somewhere
  File file_;
  Config config_;

  std::vector<std::string> parseAcceptMediaTypeHeader(const std::string &acceptMediaType) {
    std::vector<std::string> mediaTypes;
    std::istringstream ss(acceptMediaType);
    std::string token;
    while (std::getline(ss, token, ',')) {
      mediaTypes.push_back(token);
    }
    return mediaTypes;
  }

  float getQValue(const std::string &mediaTypeTag) {
    size_t pos = mediaTypeTag.find(";q=");
    if (pos != std::string::npos) {
      try {
        return std::stof(mediaTypeTag.substr(pos + 3));
      } catch (const std::exception &e) {
        return 1.0; // Default to 1.0 if parsing fails
      }
    }
    return 1.0; // Default q-value is 1.0 if not specified
  }

  std::string getBaseMediaType(const std::string &mediaTypeTag) {
    size_t pos = mediaTypeTag.find(';');
    if (pos != std::string::npos) {
      return mediaTypeTag.substr(0, pos);
    }
    return mediaTypeTag;
  }

  bool matchMediaType(const std::string &baseMediaType, const std::string &availableMediaType) {
    // Exact match
    if (baseMediaType == availableMediaType) {
      return true;
    }

    // Match with wildcard
    if (baseMediaType == "*/*" || baseMediaType == "*") {
      return true;
    }

    // Match type/*
    size_t pos = baseMediaType.find('/');
    if (pos != std::string::npos) {
      std::string baseType = baseMediaType.substr(0, pos);
      if (baseType == availableMediaType.substr(0, pos)) {
        return true;
      }
    }

    return false;
  }
};
