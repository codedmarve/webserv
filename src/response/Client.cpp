#include "../../inc/AllHeaders.hpp"

Client::Client(DB &db, Listen &host_port, HttpRequest &req_, size_t targetServerIdx, int status) : request_(&req_), host_port_(host_port), config_(NULL), response_(NULL), db_(db), serverId_(targetServerIdx), statusCode_(status)
{
  setupConfig();
  is_cgi_ = false;
}

Client::~Client()
{
  delete request_;
  delete config_;
  delete response_;
}

Client::Client(const Client &rhs) : host_port_(rhs.host_port_), db_(rhs.db_), serverId_(rhs.serverId_), statusCode_(rhs.statusCode_)
{
    if (rhs.request_)
        request_ = new HttpRequest(*rhs.request_);
    else
        request_ = NULL;

    if (rhs.response_)
        response_ = new HttpResponse(*rhs.response_);
    else
        response_ = NULL;

    if (rhs.config_)
        config_ = new RequestConfig(*rhs.config_);
    else
        config_ = NULL;
}

// Assignment operator overload
Client &Client::operator=(const Client &rhs)
{
    if (this == &rhs)
        return *this;

    delete request_;
    delete response_;
    delete config_;

    host_port_ = rhs.host_port_;
    serverId_ = rhs.serverId_;
    statusCode_ = rhs.statusCode_;

    if (rhs.request_)
        request_ = new HttpRequest(*rhs.request_);
    else
        request_ = NULL;

    if (rhs.response_)
        response_ = new HttpResponse(*rhs.response_);
    else
        response_ = NULL;
    if (rhs.config_)
        config_ = new RequestConfig(*rhs.config_);
    else
        config_ = NULL;

    return *this;
}

void Client::setupConfig()
{
  config_ = new RequestConfig(*request_, host_port_, db_, *this);
  config_->setUp(serverId_);
}
std::string Client::getResponseString()
{
  return response_->getSampleResponse();
}

void Client::setCgi(bool &val)
{
  is_cgi_ = val;
}

bool Client::getCgi()
{
  return is_cgi_;
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
    request_ = NULL;

}

HttpRequest *Client::getRequest(bool val) {
  if (!request_ && val)
    request_ = new HttpRequest();
  return request_;
}

HttpResponse *Client::getResponse() {
  return response_;
}

RequestConfig *Client::getConfig() {
  return config_;
}

RequestConfig &Client::getConfigRef() {
  return *config_;
}

HttpResponse &Client::getResponseRef() {
  return *response_;
}