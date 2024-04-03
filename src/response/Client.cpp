#include "../../inc/AllHeaders.hpp"

// Client::Client(int fd, std::string &addr, Listen &host_port, int worker_id, bool disconnect) : fd_(fd), addr_(addr), host_port_(host_port), worker_id_(worker_id), disconnect_(disconnect) {
//   request_ = NULL;
//   config_ = NULL;
//   response_ = NULL;
// }
Client::Client(Listen& host_port) : host_port_(host_port) {
  (void)host_port_;
}


Client::~Client() {
}

void Client::setupConfig(DB db, HttpRequestParser &req_, size_t requestedServerIdx) {
  (void) db;
  (void)req_;

  request_ = &req_;
  // std::cout << "****** STATUS: " << request_->getBody() <<std::endl;
  // printAllDBData(db.serversDB);

  printData(getDataByIdx(db.serversDB, requestedServerIdx));

  (void)request_;
}
