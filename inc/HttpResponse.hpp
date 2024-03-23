#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "./HttpRequestParser.hpp"

class RequestConfig;

extern pthread_mutex_t g_write;
// extern StatusCode g_status;
// extern MimeTypes g_mimes;

class Response {
 public:
  Response(RequestConfig &config, int worker_id, int error_code = 0);
  ~Response();


  int buildErrorPage(int status_code);
  

 private:

};

#endif