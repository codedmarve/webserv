#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "./AllHeaders.hpp"

struct Listen {
  std::string ip_;
  uint32_t port_;

  Listen() : ip_(""), port_(0) {};
  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port) {};
};

class ServerConfig {
 public:

  ServerConfig();
  ~ServerConfig();

  // ServerConfig	&operator=(const ServerConfig &copy);

  void listen(std::vector<std::string>::iterator &it);


 private:
  std::vector<Listen> listens_;
  // std::vector<std::string> server_name_;
  // std::vector<ServerConfig> locations_;
  // std::string uri_;
  // LocationModifier modifier_;
  // std::string credentials_;
  // bool autoindex_;
  // size_t client_max_body_size_;
  // std::string root_;
  // std::string upload_;
  // std::vector<std::string> methods_;
  // std::map<int, std::string> error_codes_;
  // std::vector<std::string> index_;
  // std::map<std::string, std::string> cgi_;
  // std::string cgi_bin_;

};

#endif
