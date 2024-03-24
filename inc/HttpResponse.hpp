#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "./AllHeaders.hpp"

class RequestConfig;

extern pthread_mutex_t g_write;

class Response {
public:
    Response(RequestConfig &config, int worker_id, int error_code = 0);
    ~Response();


    int buildErrorPage(int status_code);

    bool redirect_;
    int redirect_code_;
    std::string redirect_target_;

private:
    RequestConfig &config_;

};

#endif