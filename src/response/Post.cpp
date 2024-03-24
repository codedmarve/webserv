#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <cstring>
#include <pthread.h> // For pthread_mutex_t
#include <unistd.h> // For sleep
#include "file.h"   // Assuming File class definition
#include "config.h" // Assuming Config class definition

// Mutex for thread safety
pthread_mutex_t g_write = PTHREAD_MUTEX_INITIALIZER;

class Response {
public:
  int POST(const std::string& accept_header) {
    body_ = config_.getBody();

    pthread_mutex_lock(&g_write);

    if (!file_.exists()) {
      file_.create(body_);
      return 201; // Created
    } else {
      file_.append(body_);
      return 200; // OK
    }

    pthread_mutex_unlock(&g_write);

    // The following lines will not be reached due to the early return
    headers_["Content-Length"] = ft::to_string(body_.length());
    headers_["Content-Type"] = negotiateContentType(accept_header); // Content negotiation
  }

private:
  std::map<std::string, std::string> headers_;
  std::string body_;
  std::string response_;

  // Assume these are defined somewhere
  File file_;
  Config config_;

  std::string getCurrentDateTime() {
    char buffer[80];
    std::time_t now = std::time(NULL);
    std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return buffer;
  }

  std::string negotiateContentType(const std::string& accept_header) {
    // Default to text/plain if negotiation fails
    std::string defaultContentType = "text/plain";

    std::istringstream iss(accept_header);
    std::string token;

    std::vector<std::string> clientMediaTypes;
    std::vector<std::pair<std::string, double> > clientTypeQValues;

    while (std::getline(iss, token, ',')) {
      size_t paramPos = token.find(';');
      std::string mediaType = (paramPos != std::string::npos) ? token.substr(0, paramPos) : token;
      std::string qValueStr = (paramPos != std::string::npos) ? token.substr(paramPos + 1) : "";
      double qValue = 1.0; // Default q value

      size_t qPos = qValueStr.find("q=");
      if (qPos != std::string::npos) {
        std::string qStr = qValueStr.substr(qPos + 2);
        double q = 0.0;
        std::istringstream(qStr) >> q;
        if (!q) {
          qValue = 0.0;
        } else {
          qValue = q;
        }
      }

      clientMediaTypes.push_back(mediaType);
      clientTypeQValues.push_back(std::make_pair(mediaType, qValue));
    }

    // Example supported media types by the server
    std::vector<std::string> serverSupportedTypes;
    serverSupportedTypes.push_back("application/json");
    serverSupportedTypes.push_back("text/html");
    serverSupportedTypes.push_back("text/plain");

    // Sort the client types by q values (higher q value means higher preference)
    std::sort(clientTypeQValues.begin(), clientTypeQValues.end(), CompareQValues());

    // Check if any of the client's types match the server's supported types
    for (size_t i = 0; i < clientMediaTypes.size(); ++i) {
      for (size_t j = 0; j < serverSupportedTypes.size(); ++j) {
        if (clientMediaTypes[i] == serverSupportedTypes[j]) {
          return clientMediaTypes[i]; // Return the first match
        }
      }
    }

    return defaultContentType; // No match found, return default
  }

  // Function object for comparing q-values
  struct CompareQValues {
    bool operator()(const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) const {
      return a.second > b.second;
    }
  };
};
