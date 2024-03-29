#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(HttpRequestParser &request) : request_(request){}

RequestConfig::~RequestConfig() {}


void RequestConfig::setTarget(std::string target) {
  target_ = target;
}

std::string &RequestConfig::getTarget() {
  return target_;
}

