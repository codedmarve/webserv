#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP

# include "./AllHeaders.hpp"

class HttpRequestParser;
class Client;
struct Listen;
struct DB;

class RequestConfig {
 public:
  RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client);
  ~RequestConfig();

  void setUp();
  void setTarget(const std::string &target);


 private:
  HttpRequestParser &request_;
  Client &client_;
  Listen &host_port_;
  DB &db_;
  
  std::string target_;
};

#endif