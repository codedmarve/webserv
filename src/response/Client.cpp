#include "../../inc/AllHeaders.hpp"

// Client::Client(int fd, std::string &addr, Listen &host_port, int worker_id, bool disconnect) : fd_(fd), addr_(addr), host_port_(host_port), worker_id_(worker_id), disconnect_(disconnect) {
//   request_ = NULL;
//   config_ = NULL;
//   response_ = NULL;
// }
Client::Client(Listen& host_port) : host_port_(host_port), config_(NULL) {
  (void)host_port_;
}


Client::~Client() {
  delete config_;
}

void Client::setupConfig(DB db, HttpRequestParser &req_, size_t requestedServerIdx) {
  (void) db;
  (void)req_;
  (void)request_;
  (void) requestedServerIdx;

  request_ = &req_;
  // std::cout << "****** STATUS: " << request_->getBody() <<std::endl;
  // printAllDBData(db.serversDB);
  // printData(getDataByIdx(db.serversDB, requestedServerIdx));


  // **** AGGREGATE CONFIG HERE
  config_ = new RequestConfig(*request_, host_port_, db, *this);
  config_->setUp();
}

// RequestConfig* Client::createRequestConfig(DB db) {
//     // Create and return a new RequestConfig object
//     return ;
// }