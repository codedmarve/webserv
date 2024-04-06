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

  void setUp(size_t targetServerIdx);
  void setTarget(const std::string &target);
  std::vector<std::string> filterDataByDirectives(const std::vector<KeyMapValue>& values, std::string directive, std::string location);
  std::string checkModifier(const std::string& modifiers);


  std::vector<KeyMapValue> targetServer_;

  std::string &getTarget();
  std::string &getRequestTarget();
  std::string &getQuery();
  std::string &getHost();
  uint32_t &getPort();


 private:
  HttpRequestParser& request_;
  Client &client_;
  Listen &host_port_;
  DB &db_;
  LocationModifier modifierType_;
  
  std::string target_;
};

#endif