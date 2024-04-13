#include "../../inc/AllHeaders.hpp"

Client::Client(DB &db, Listen &host_port, HttpRequestParser &req_, size_t targetServerIdx, int status) : request_(&req_), host_port_(host_port), config_(NULL), response_(NULL), db_(db),  serverId_(targetServerIdx), statusCode_(status)
{
  setupConfig();
}

Client::~Client()
{
  delete config_;
}

void Client::setupConfig()
{
  // (void)targetServerIdx;

  // request_ = &req_;
  // std::cout << "****** STATUS: " << request_->getBody() <<std::endl;
  // printAllDBData(db.serversDB);
  // printData(getDataByIdx(db.serversDB, requestedServerIdx));
  // (void)db;

  /// @note AGGREGATED ALL CONFIGURATIONS HERE
  config_ = new RequestConfig(*request_, host_port_, db_, *this);
  config_->setUp(serverId_);
  CgiHandle cgi(config_);
}

void Client::setupResponse()
{
  if (!request_)
    request_ = new HttpRequestParser();

  if (!config_)
    setupConfig();

  response_ = new HttpResponse(*config_, statusCode_);

  int loop = 0;

  for (int ret = 1; ret != 0; loop++) {
    ret = 0;
    std::cout << "HEREEEEEE\n";
    response_->build();
    exit(1);
  }
}