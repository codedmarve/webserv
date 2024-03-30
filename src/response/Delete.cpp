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
//   int DELETE() {
//     pthread_mutex_lock(&g_write);

//     int status_code = 200; // Default to OK

//     if (!file_.exists()) {
//       status_code = 404; // Not Found
//     } else {
//       file_.unlink();
//       status_code = 200; // OK
//       body_ += "<!DOCTYPE html>\n\
//                 <html>\n\
//                 <body>\n\
//                   <h1>File deleted</h1>\n\
//                 </body>\n\
//                 </html>";
//       headers_["Content-Type"] = "text/html";
//       headers_["Content-Length"] = ft::to_string(body_.length());
//     }

//     headers_["Date"] = getCurrentDateTime();

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
