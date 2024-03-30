// #include <iostream>
// #include <sstream>
// #include <fstream>
// #include <ctime>
// #include <map>
// #include <cstring>
// #include <pthread.h> // For pthread_mutex_t
// #include <unistd.h>  // For sleep
// #include "file.h"    // Assuming File class definition
// #include "config.h"  // Assuming Config class definition

// // Mutex for thread safety
// pthread_mutex_t g_write = PTHREAD_MUTEX_INITIALIZER;

// class Response {
// public:
//   int PUT() {
//     pthread_mutex_lock(&g_write);

//     int status_code = 204; // Default to No Content

//     if (!file_.exists()) {
//       file_.create(config_.getBody());
//       status_code = 201; // Created
//       headers_["Content-Length"] = "0";
//     } else {
//       if (file_.update(config_.getBody())) {
//         status_code = 200; // OK
//       } else {
//         status_code = 409; // Conflict, if update fails
//       }
//     }

//     headers_["Date"] = getCurrentDateTime();
//     headers_["Content-Type"] = "text/plain"; // Default Content-Type
//     headers_["Content-Length"] = "0"; // Reset Content-Length

//     pthread_mutex_unlock(&g_write);

//     return status_code;
//   }

// private:
//   std::map<std::string, std::string> headers_;
//   std::string body_;
//   std::string response_;

//   // Assume these are defined somewhere
//   File file_;
//   Config config_;

//   std::string getCurrentDateTime() {
//     char buffer[80];
//     std::time_t now = std::time(NULL);
//     std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
//     return buffer;
//   }
// };
