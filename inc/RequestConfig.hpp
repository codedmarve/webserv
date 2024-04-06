#ifndef REQUESTCONFIG_HPP
# define REQUESTCONFIG_HPP

# include "./AllHeaders.hpp"

class HttpRequestParser;
class Client;
struct Listen;
struct DB;

enum LocationModifier {
  NONE,
  EXACT,
  CASE_SENSITIVE,
  CASE_INSENSITIVE,
  LONGEST,
};


typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr > KeyValues;
typedef std::pair<MapStr, VecStr > KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue > > GroupedDBMap;

class RequestConfig {
 public:
  RequestConfig(HttpRequestParser& request, Listen &host_port, DB &db, Client &client);
  ~RequestConfig();
  std::vector<std::string> filterDataByDirectives(const std::vector<KeyMapValue>& values, std::string directive, std::string location);

  std::vector<std::string> cascadeFilter(const std::vector<KeyMapValue>& values, std::string directive, std::string location);

  void setUp(size_t targetServerIdx);
  void setTarget(const std::string &target);
  std::string checkModifier(const std::string& modifiers);


  std::vector<KeyMapValue> targetServer_;

  std::string &getTarget();
  std::string &getRequestTarget();
  std::string &getQuery();
  std::string &getHost();
  uint32_t &getPort();
  Client &getClient();
  std::string &getRoot();
  std::string &getUri();
  void setRoot(const std::string root);
  void setUri(const std::string uri);
  size_t &getClientMaxBodySize();
  void setClientMaxBodySize(std::string size);


 private:
  HttpRequestParser& request_;
  Client &client_;
  Listen &host_port_;
  DB &db_;
  LocationModifier modifierType_;
  
  std::string target_;
  std::string root_;
  std::string uri_;
  size_t client_max_body_size_;
};

#endif