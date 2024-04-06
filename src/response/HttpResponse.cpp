#include "../../inc/AllHeaders.hpp"

Response::Response(RequestConfig &config, int error_code) : config_(config), error_code_(error_code) {

  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_code_ = 0;
  redirect_ = false;
  charset_ = "";
  // initMethodMap();
}

Response::~Response() {}

void Response::clear() {
  error_code_ = 0;
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_ = false;
  response_.clear();
  body_.clear();
  headers_.clear();
}


void Response::initMethodMap() {
  // methods_["GET"] = &Response::GET;
  // methods_["HEAD"] = &Response::GET;
  // methods_["POST"] = &Response::POST;
  // methods_["PUT"] = &Response::PUT;
  // methods_["DELETE"] = &Response::DELETE;
}

// int Response::GET() {
  
//   if (config_.getAutoindex() && file_.is_directory()) {
//     headers_["Content-Type"] = g_mimes.getType(".html");
//     body_ = file_.autoIndex(config_.getRequestTarget());
//     headers_["Content-Length"] = ft::to_string(body_.length());
//   }
//   else {
//     headers_["Content-Type"] = g_mimes.getType(file_.getMimeExtension());
//     if (!charset_.empty())
//       headers_["Content-Type"] += "; charset=" + charset_;
//     body_ = file_.getContent();
//     headers_["Content-Length"] = ft::to_string(body_.length());
//   }
//   return 200;
// }