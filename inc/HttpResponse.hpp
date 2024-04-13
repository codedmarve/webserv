#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "./AllHeaders.hpp"
#include "./File.hpp"

class HttpRequestParser;
class RequestConfig;
class File;

extern pthread_mutex_t g_write;

class HttpResponse {
public:
    HttpResponse(RequestConfig &config, int error_code = 100);
    ~HttpResponse();

    typedef int (HttpResponse::*type)();
    void initMethodMap();

    enum LogLevel {
    NONE,
    INFO,
    DEBUG
    };

    void clear();
    // int buildErrorPage(int status_code);
    // bool isCGI(std::string extension);
    // void build();
    // int handleMethods();
    // void createResponse();
    // bool checkAuth();
    // bool localization(std::vector<std::string> &matches);
    // std::string accept_charset(std::vector<std::string> &matches);
    // std::string buildMethodList();
    // bool shouldDisconnect();
    // bool redirect();
    // std::string redirect_target();
    // std::string response_log(LogLevel level);
    // std::string getCurrentDateTime();
    // void logError(const std::string& message);

    int GET();
    int POST();
    int PUT();
    int DELETE();

    int getStatus();
    std::string getResponseBody();
    int send(int fd);

    void build();


private:
    RequestConfig &config_;
    File *file_;
    int error_code_;
    int worker_id_;
    size_t total_sent_;
    int status_code_;
    std::string response_;
    std::string body_;
    bool redirect_;
    std::string redirect_target_;
    int redirect_code_;
    size_t header_size_;
    size_t body_size_;
    std::string charset_;
    std::map<std::string, HttpResponse::type> methods_;
    std::map<std::string, std::string> headers_;   

    std::string buildMethodList();
    bool checkAuth();
};

std::string getMimeType(const std::string& ext);
std::string getHttpStatusCode(int code);


#endif