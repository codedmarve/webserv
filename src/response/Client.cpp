#include "../../inc/AllHeaders.hpp"

Client::Client(DB &db, Listen &host_port, HttpRequest &req_, size_t targetServerIdx, int status) : request_(&req_), host_port_(host_port), config_(NULL), response_(NULL), db_(db), serverId_(targetServerIdx), statusCode_(status)
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
  // CgiHandle cgi(config_);
}
std::string Client::getResponseString()
{
  return response_->getSampleResponse();
}

void Client::setupResponse()
{
  if (!request_)
    request_ = new HttpRequest();

  if (!config_)
    setupConfig();

  response_ = new HttpResponse(*config_, statusCode_);

  int loop = 0;

  for (int ret = 1; ret != 0; loop++)
  {
    ret = 0;
    response_->build();

    if (response_->getRedirect())
    {
      config_->redirectLocation(response_->redirect_target());
      std::cout << "REDIRECTED TO: " << response_->redirect_target() << std::endl;
      response_->cleanUp();
      ret = 1;
    }
    if (loop >= 10)
    {
      if (response_)
      {
        delete response_;
        response_ = NULL;
      }
      response_ = new HttpResponse(*config_, 500);
      response_->build();
      break;
    }
  }
  if (request_)
  {
    // delete request_;
    request_ = NULL;
  }
}

HttpRequest *Client::getRequest(bool val) {
  if (!request_ && val)
    request_ = new HttpRequest();
  return request_;
}

HttpResponse *Client::getResponse() {
  return response_;
}
