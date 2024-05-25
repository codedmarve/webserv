#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "AllHeaders.hpp"
#include "Listen.hpp"
#include "DB.hpp"

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
    Client &operator=(const Client &rhs);
    Client(const Client &rhs);
    
    void setupConfig();
    void setupResponse();
    HttpRequest *getRequest(bool val = false);
    HttpResponse *getResponse();
    RequestConfig *getConfig();
    std::string getResponseString();
    void setCgi(bool &val);
    bool getCgi();
    RequestConfig &getConfigRef();
    HttpResponse &getResponseRef();
    HttpRequest &getRequestRef();
    Client* clone() const;

private:
    HttpRequest *request_;
    Listen host_port_;
    RequestConfig *config_;
    HttpResponse *response_;
    DB db_;
    size_t serverId_;
    int statusCode_;
    bool is_cgi_;
};

#endif