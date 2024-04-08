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

void Response::build() {
  // std::string &method = config_.getMethod();

//   file_.set_path(config_.getRoot() + "/" + config_.getTarget());

//   if (error_code_ > 200)
//     status_code_ = error_code_;
//   else if (!config_.isMethodAccepted(method)) {
//     status_code_ = 405;
//     headers_["Allow"] = buildMethodList();
//   }
//   else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
//     status_code_ = 413;
//   }
//   else if (config_.getAuth() != "off" && !checkAuth())
//     status_code_ = 401;
//   else
//     status_code_ = handleMethods();

//   if (status_code_ >= 300 && !body_.length())
//     status_code_ = buildErrorPage(status_code_);

//   if (!redirect_)
//     createResponse();
}

bool Response::checkAuth() {
  std::string authCredentials = config_.getHeader("Authorization");
  if (authCredentials.empty())
    return false;
  std::string token = b64decode(authCredentials.substr(authCredentials.find(' ') + 1));
  return (token == config_.getAuth());
}

std::string Response::buildMethodList() {
  std::string list;
  std::vector<std::string> methods = config_.getMethods();

  std::vector<std::string>::iterator it = methods.begin();

  while (it != methods.end()) {
    list += *it;
    it++;
    if (it != methods.end())
      list += ", ";
  }
  return list;
}