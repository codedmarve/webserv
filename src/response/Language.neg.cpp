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
  bool localization(std::vector<std::string> &matches) {
    std::string acceptLanguage = config_.getHeader("Accept-Language");
    std::vector<std::string> requestLanguages = parseAcceptLanguageHeader(acceptLanguage);

    // Default to "en" if no matching language is found
    std::string selectedLanguage = "en";

    float maxQValue = 0.0;

    // Iterate over each requested language
    for (const std::string &requestLanguage : requestLanguages) {
      float qValue = getQValue(requestLanguage);
      std::string baseLanguage = getBaseLanguage(requestLanguage);

      // Find the best matching language
      for (const std::string &availableLanguage : matches) {
        if (matchLanguage(baseLanguage, availableLanguage) && qValue > maxQValue) {
          selectedLanguage = availableLanguage;
          maxQValue = qValue;
        }
      }
    }

    // Set the Content-Language header to the selected language
    headers_["Content-Language"] = selectedLanguage;

    // Return true if a language was successfully negotiated
    return (selectedLanguage != "en");
  }

private:
  std::map<std::string, std::string> headers_;
  std::string body_;
  std::string response_;

  // Assume these are defined somewhere
  File file_;
  Config config_;

  std::vector<std::string> parseAcceptLanguageHeader(const std::string &acceptLanguage) {
    std::vector<std::string> languages;
    std::istringstream ss(acceptLanguage);
    std::string token;
    while (std::getline(ss, token, ',')) {
      languages.push_back(token);
    }
    return languages;
  }

  float getQValue(const std::string &languageTag) {
    size_t pos = languageTag.find(";q=");
    if (pos != std::string::npos) {
      try {
        return std::stof(languageTag.substr(pos + 3));
      } catch (const std::exception &e) {
        return 1.0; // Default to 1.0 if parsing fails
      }
    }
    return 1.0; // Default q-value is 1.0 if not specified
  }

  std::string getBaseLanguage(const std::string &languageTag) {
    size_t pos = languageTag.find(';');
    if (pos != std::string::npos) {
      return languageTag.substr(0, pos);
    }
    return languageTag;
  }

  bool matchLanguage(const std::string &baseLanguage, const std::string &availableLanguage) {
    // Exact match
    if (baseLanguage == availableLanguage) {
      return true;
    }

    // Match with wildcard
    if (baseLanguage.length() > 1 && baseLanguage.substr(2) == "-*") {
      std::string basePrefix = baseLanguage.substr(0, 2);
      std::string availablePrefix = availableLanguage.substr(0, 2);
      return basePrefix == availablePrefix;
    }

    return false;
  }
};
