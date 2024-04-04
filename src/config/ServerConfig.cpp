#include "../../inc/AllHeaders.hpp"

ServerConfig::ServerConfig() {
}

ServerConfig::~ServerConfig() {
}

// void ServerConfig::listen(std::vector<std::string>::iterator &it) {
//   std::string str = *it;
//   (void) str;
//   std::string ip = "0.0.0.0";
//   uint32_t port = 8000;

//   if (str.find(':') != std::string::npos) {
//     ip = str.substr(0, str.find(':'));

//     std::string port_str = str.substr(str.find(':') + 1);

//     if (port_str.find_first_not_of("0123456789") != std::string::npos) {
//       throw std::runtime_error("invalid port :" + port_str);
//     }
    
//     std::istringstream iss(port_str); 
//     iss >> port;
//     if (port > 65535)
//       throw std::runtime_error("duplicate value in 'listen'");
//   }
//   else if (str.find_first_not_of("0123456789") != std::string::npos) {
//     ip = str;
//   } else {
//     std::istringstream iss(str); 
//     iss >> port;
//   }

//   Listen list(ip, port);
// }


