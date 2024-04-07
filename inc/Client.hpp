#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "AllHeaders.hpp"

# define START_TIMEOUT 300
# define LAST_TIMEOUT 150

typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr > KeyValues;
typedef std::pair<MapStr, VecStr > KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue > > GroupedDBMap;

class RequestConfig;
class InputArgs;
class Response;
struct DB;


class Client {
 public:
  Client(DB &db, Listen &host_port, HttpRequestParser &req_, size_t targetServerIdx);
  ~Client();

  void setupConfig();

 private:
    HttpRequestParser *request_;
    Listen &host_port_;
    RequestConfig *config_;
    Response *response_;
    DB &db_;
    size_t serverId_;

    void setupResponse();
};

#endif