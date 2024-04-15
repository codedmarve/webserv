#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AllHeaders.hpp"

#define START_TIMEOUT 300
#define LAST_TIMEOUT 150

typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr> KeyValues;
typedef std::pair<MapStr, VecStr> KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue> > GroupedDBMap;

class RequestConfig;
class InputArgs;
class HttpResponse;
struct DB;

class Client
{
public:
    Client(DB &db, Listen &host_port, HttpRequest &req_, size_t targetServerIdx, int status);
    ~Client();

    void setupConfig();
    void setupResponse();
    HttpRequest *getRequest(bool val = false);
    HttpResponse *getResponse();
    std::string getResponseString();

private:
    HttpRequest *request_;
    Listen &host_port_;
    RequestConfig *config_;
    HttpResponse *response_;
    DB &db_;
    size_t serverId_;
    int statusCode_;
};

#endif