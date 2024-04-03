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
  Client();
  // Client(int fd, std::string &addr, Listen &host_port, int worker_id, bool disconnect = false);
  ~Client();

  void setupConfig(DB db, HttpRequestParser &req_);

 private:
    HttpRequestParser *request_;
    // RequestConfig *config_;

};

#endif