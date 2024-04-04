#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequestParser &request, Listen &host_port, DB &db, Client &client) : request_(request), client_(client), host_port_(host_port), db_(db) {
}

RequestConfig::~RequestConfig() {}

void RequestConfig::setUp() {
    std::vector<KeyMapValue> targetServer = getDataByIdx(db_.serversDB, 0);
    // printData(targetServer);
    // std::cout << "**********" << request_.getStatus() << std::endl;
    // if (request_.getStatus() > 2)
}


// void RequestConfig::setTarget(std::string target) {
//   target_ = target;
// }

// std::string &RequestConfig::getTarget() {
//   return target_;
// }

