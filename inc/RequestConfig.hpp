#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP



# include "./AllHeaders.hpp"

class HttpRequestParser;

class RequestConfig {
 public:
  RequestConfig(HttpRequestParser &request);
  ~RequestConfig();

    enum LogLevel {
        NONE,
        INFO,
        DEBUG
    };


  void redirectLocation(std::string target);
  bool methodAccepted(std::string &method);
  std::string &getMethod();
  std::string &getRoot();
  std::string &getUri();
  std::string &getTarget();
  std::string &getRequestTarget();
  std::string &getQuery();
  std::string &getCGIBin();
  void setTarget(std::string target);
  std::string &getBody();
  std::string &getAuth();
  std::map<std::string, std::string> &getCGI();
  // std::map<std::string, std::string, s_compare> &getHeaders();
  std::string &getHeader(std::string key);
  size_t &getClientMaxBodySize();
  std::string &getProtocol();
  std::string &getUpload();
  bool getAutoindex();
  std::vector<std::string> &getIndexes();
  std::map<int, std::string> &getErrorPages();
  std::vector<std::string> &getMethods();
  std::string &getHost();
  uint32_t &getPort();
  std::string log(LogLevel level);

 private:
  HttpRequestParser &request_;
//   Listen &host_port_;
//   std::vector<ServerConfig> &servers_;
//   Client &client_;
//   ServerConfig *server_;
//   ServerConfig *location_;
  std::string target_;
};

#endif