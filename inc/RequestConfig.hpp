#ifndef REQUESTCONFIG_HPP
#define REQUESTCONFIG_HPP

#include "./AllHeaders.hpp"

class HttpRequestParser;
class Client;
struct Listen;
struct DB;

enum LocationModifier
{
  NONE,
  EXACT,
  CASE_SENSITIVE,
  CASE_INSENSITIVE,
  LONGEST,
};

typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr> KeyValues;
typedef std::pair<MapStr, VecStr> KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue> > GroupedDBMap;

class RequestConfig
{
public:
  RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client);
  ~RequestConfig();
  const VecStr &filterDataByDirectives(const std::vector<KeyMapValue> &values, std::string directive, std::string location);
  const VecStr &cascadeFilter(std::string directive, std::string location);
  const VecStr &checkRootDB(std::string directive);
  std::string checkModifier(const std::string &modifiers);
  bool isMethodAccepted(std::string &method);
  void redirectLocation(std::string target);

  void setUp(size_t targetServerIdx);
  void setTarget(const std::string &target);
  void setRoot(const VecStr root);
  void setUri(const std::string uri);
  void setClientMaxBodySize(const VecStr size);
  void setAutoIndex(const VecStr autoindex);
  void setIndexes(const VecStr &indexes);
  void setErrorPages(const VecStr &errors);
  void setMethods(const VecStr &errors);

  std::string &getTarget();
  std::string &getRequestTarget();
  std::string &getQuery();
  std::string &getFragment();
  std::string &getHost();
  uint32_t &getPort();
  Client &getClient();
  std::string &getRoot();
  std::string &getUri();
  size_t &getClientMaxBodySize();
  bool getAutoIndex();
  VecStr &getIndexes();
  std::map<int, std::string> &getErrorPages();
  std::vector<std::string> &getMethods();
  std::string &getMethod();
  std::string &getBody();
  std::map<std::string, std::string> getHeaders();
  std::string &getHeader(std::string key);
  std::string &getProtocol();

  void printConfigSetUp();

private:
  HttpRequestParser &request_;
  std::vector<KeyMapValue> targetServer_;
  Client &client_;
  Listen &host_port_;
  DB &db_;
  LocationModifier modifierType_;

  std::string target_;
  std::string root_;
  std::string uri_;
  size_t client_max_body_size_;
  bool autoindex_;
  std::vector<std::string> indexes_;
  std::map<int, std::string> error_codes_;
  std::vector<std::string> methods_;
  size_t serverId;
};

#endif
