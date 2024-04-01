#include "../../inc/AllHeaders.hpp"

ServerConfig::ServerConfig() {
}

ServerConfig::~ServerConfig() {
}

// ServerConfig	&ServerConfig::operator=(const ServerConfig &copy) {
//   client_max_body_size_ = copy.client_max_body_size_;
//   root_ = copy.root_;
//   error_codes_ = copy.error_codes_;
//   index_ = copy.index_;
//   cgi_ = copy.cgi_;
//   cgi_bin_ = copy.cgi_bin_;
//   autoindex_ = copy.autoindex_;
//   credentials_ = copy.credentials_;
//   return (*this);
// }

// std::map<std::string, ServerConfig::type> ServerConfig::directive_;
// void ServerConfig::initDirectiveMap() {
//   ServerConfig::directive_["listen"] = &ServerConfig::listen;
// }




void ServerConfig::listen(std::vector<std::string>::iterator &it) {
  std::string str = *it;
  (void) str;
  std::string ip = "0.0.0.0";
  uint32_t port = 8000;

// 0.0.0.0:8080

  if (str.find(':') != std::string::npos) {
    ip = str.substr(0, str.find(':'));

    std::string port_str = str.substr(str.find(':') + 1);

    if (port_str.find_first_not_of("0123456789") != std::string::npos) {
      throw std::runtime_error("invalid port :" + port_str);
    }
    
    std::istringstream iss(port_str); 
    iss >> port;
    if (port > 65535)
      throw std::runtime_error("duplicate value in 'listen'");
  }
  else if (str.find_first_not_of("0123456789") != std::string::npos) {
    ip = str;
  } else {
    std::istringstream iss(str); 
    iss >> port;
  }

  Listen list(ip, port);

//   if (std::find(listens_.begin(), listens_.end(), list) != listens_.end())
//     throw std::runtime_error("duplicate value in 'listen'");
//     // throw webserv_exception("duplicate listen " + ip + ":" + port);

//   listens_.push_back(Listen(ip, port));

//   if (*++it != ";")
//     throw std::runtime_error("double value in 'listen'");
}