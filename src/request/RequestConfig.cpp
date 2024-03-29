#include "../../inc/AllHeaders.hpp"

RequestConfig::RequestConfig(Request &request) : request_(request){}

RequestConfig::~RequestConfig() {}


void RequestConfig::setTarget(std::string target) {
  target_ = target;
}

std::string &RequestConfig::getTarget() {
  std::cout << target_ << " <<= DEBUG 1\n";
  return target_;
}
