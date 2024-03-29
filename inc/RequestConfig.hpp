#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP



// # include "InputArgs.hpp"
// # include "Client.hpp"
// # include "Utils.hpp"
# include "../../inc/AllHeaders.hpp"

// class Client;
// class InputArgs;
class Request;
// class ServerConfig;
// struct Listen;

class RequestConfig {
 public:
  RequestConfig(Request &request);
  ~RequestConfig();

    enum LogLevel {
        NONE,
        INFO,
        DEBUG
    };

//   void setup(InputArgs &options);

//   ServerConfig *getServerForRequest(std::vector<ServerConfig> &servers);
//   ServerConfig *getLocationForRequest(ServerConfig *server, std::string &target);
//   Client &getClient();
//   ServerConfig *match_regexp(std::vector<ServerConfig*> &locations, std::string &target);


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
  std::map<std::string, std::string, s_compare> &getHeaders();
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
  Request &request_;
//   Listen &host_port_;
//   std::vector<ServerConfig> &servers_;
//   Client &client_;
//   ServerConfig *server_;
//   ServerConfig *location_;
  std::string target_;
};

#endif