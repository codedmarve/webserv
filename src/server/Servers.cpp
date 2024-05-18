/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:07 by alappas           #+#    #+#             */
/*   Updated: 2024/05/05 21:48:18 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Servers.hpp"
#include "../../inc/HttpRequest.hpp"

//Servers constuctor
Servers::Servers(ConfigDB &configDB) : _server_fds(), configDB_(configDB){
	_keyValues = configDB_.getKeyValue();
	createServers();
	initEvents();
}

//Servers destructor
Servers::~Servers() {
	for (std::vector<int>::iterator it = _server_fds.begin(); it != _server_fds.end(); ++it)
		close(*it);
	close(_epoll_fds);
}

// Create socket
int Servers::createSocket(){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Socket creation failed" << std::endl;
		return (0);
	}
	_server_fds.push_back(server_fd);
	return (1);
}

// Bind socket
int Servers::bindSocket(std::string s_port){
	if (_server_fds.back() == -1)
	{
		std::cerr << "Socket binding impossible!" << std::endl;
		return (0);
	}
	std::stringstream ss;
	ss << s_port;
	int port;
	std::string ip_string;
	const char *c_ip = NULL;
	if (s_port.find(":") == std::string::npos)
	{
		port = std::atoi(s_port.c_str());
		_ip_to_server[_server_fds.back()] = "127.0.0.1:" + s_port;
	}
	else
	{
		_ip_to_server[_server_fds.back()] = s_port;
		getline(ss, ip_string, ':');
		ss >> port;
		c_ip = ip_string.c_str();
	}
	int opt = 1;
	setsockopt(_server_fds.back(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	if (c_ip != NULL)
		address.sin_addr.s_addr = inet_addr(c_ip);
	else
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	address.sin_port = htons(port);
	if (bind(_server_fds.back(), (struct sockaddr *)&address, sizeof(address)) == -1) {
		std::cerr << "Bind failed" << std::endl;
		return (0);
	}
	for (std::map<int, std::vector<std::string> >::iterator it = server_index.begin(); it != server_index.end(); it++){
		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
			if (*it2 == s_port)
				server_fd_to_index[_server_fds.back()] = it->first;
		}
	}
	return (1);
}


// Create epoll instance
void	Servers::createEpoll(){
	int epoll_fd = epoll_create1(0);
	this->_epoll_fds = epoll_fd;
	if (epoll_fd < 0) {
		std::cerr << "Epoll creation failed" << std::endl;
		exit(1);
	}
}

// Listen on socket
int Servers::listenSocket(){
	if (listen(_server_fds.back(), SOMAXCONN) == -1) {
		std::cerr << "Listen failed" << std::endl;
		return (0);
	}
	return (1);
}

// Combine file descriptors into epoll instance
int Servers::combineFds(){
	int flags = fcntl(_server_fds.back(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (flags == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		return (0);
	}
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN;
	event.data.fd = _server_fds.back();
	if (epoll_ctl(this->_epoll_fds, EPOLL_CTL_ADD, _server_fds.back(), &event) == -1) {
		std::cerr << "Epoll_ctl failed" << std::endl;
		return (0);
	}
	return (1);
}

// Get file descriptors for servers
void Servers::createServers(){
	
	std::cout << "Creating servers" << std::endl;
	std::vector<std::string> ports;
	createEpoll();
	ports = getPorts();
	for (std::vector<std::string>::iterator it2 = ports.begin(); it2 != ports.end(); it2++) {
		if (!checkSocket(*it2)){
			if (createSocket()){
				if (!bindSocket(*it2) || !listenSocket() || !combineFds())
					_server_fds.pop_back();
				else
				{
					assignDomain(*it2, _server_fds.back());
					std::cout << "Server created on port " << _ip_to_server[_server_fds.back()] << ", server:" << _server_fds.back() << std::endl;
				}
			}
		}
	}
}

Listen getTargetIpAndPort(std::string requestedUrl) {
	size_t pos = requestedUrl.find(":");
	Listen listen;

	if (requestedUrl.empty())
		return (std::cout << "ERROR: empty host and port" <<std::endl, listen);

	if ( pos == std::string::npos)	
		return (std::cout << "Warning: default host and port used 127.0.0.1:80" <<std::endl, listen);

	std::string x_ip = requestedUrl.substr(0, pos);
	std::string x_port = requestedUrl.substr(pos + 1);
	uint32_t port_x;
	std::istringstream iss(x_port); 
	iss >> port_x;
	return Listen (x_ip, port_x);
}

// Handle incoming connection from clients
void Servers::handleIncomingConnection(int server_fd){
	struct sockaddr_in address;
	// struct timeval timeout;
	std::string request;
    socklen_t addrlen = sizeof(address);
    char ip[INET_ADDRSTRLEN];
    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
	bool finish = false;
    if (new_socket == -1) {
        std::cerr << "Accept failed." << std::endl;
        return;
    }
	if (inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
    	std::cerr << "inet_ntop failed with error: " << strerror(errno) << std::endl;
		close(new_socket);
    	return;
}
    std::cout << "Connection established on IP: " << _ip_to_server[server_fd] << ", server:" << server_fd << "\n" << std::endl;
	int flags = fcntl(new_socket, F_SETFL, FD_CLOEXEC, O_NONBLOCK);//COMMENT THE 0_NONBLOCK LINE IF THE BEHAVIOUR IS UNDEFINED.
	if (flags == -1)
	{
		std::cerr << "Fcntl failed" << std::endl;
		close(new_socket);
		return;
	}
	HttpRequest parser;

	int reqStatus = -1;
	while (!finish){	
		finish = getRequest(new_socket, request);
		reqStatus = parser.parseRequest(request);
		if (reqStatus != 200) {
			finish = true;
		}
		if (!handleResponse(reqStatus, server_fd, new_socket, parser))
			return;
	}
    if (close(new_socket) == -1)
		std::cerr << "Close failed with error: " << strerror(errno) << std::endl;
}

// Initialize events that will be handled by epoll
void Servers::initEvents(){
	while (true){
		try{
			struct epoll_event events[_server_fds.size()];
			int n = epoll_wait(this->_epoll_fds, events, _server_fds.size(), -1);
			if (n == -1) {
				std::cerr << "Epoll_wait failed" << std::endl;
				return ;
			}
			for (int i = 0; i < n; i++) {
				for (std::vector<int>::iterator it2 = _server_fds.begin(); it2 != _server_fds.end(); ++it2) {
					if (events[i].data.fd == *it2) {
						std::cout << "\nIncoming connection on server: " << *it2 << std::endl;
						handleIncomingConnection(*it2);
					}
				}
			}
		} catch (std::exception &e){
			std::cerr << e.what() << std::endl;
		}
	}
}

// Getting ports from config file
std::vector<std::string> Servers::getPorts(){
	
	std::map<std::string, std::vector<std::string> > config = getKeyValue();
	std::stringstream ss;
	std::vector<std::string> ports_temp;
	std::vector<std::string> ports;

	for (int i = 0; i != std::numeric_limits<int>::max(); i++){
		ss.str("");
		ss << i;
		std::string server_name = "server[" + ss.str() + "]";
		std::string server;
		std::map<std::string, std::vector<std::string> >::iterator it_server_name = config.end();
		for (std::map<std::string, std::vector<std::string> >::iterator it = config.begin(); it != config.end(); it++)
		{
			if (it->first.find(server_name) != std::string::npos){
				std::size_t pos = it->first.find(server_name);
				it_server_name = it;
				std::string prefix = it->first.substr(0, pos + server_name.length());
				server = prefix + ".listen";
				break;
			}
		}
		std::map<std::string, std::vector<std::string> >::iterator it_server = config.find(server);
		if (it_server != config.end()){
			ports_temp = it_server->second;
			for (std::vector<std::string>::iterator it2 = ports_temp.begin(); it2 != ports_temp.end(); it2++){
				if (std::find(ports.begin(), ports.end(), *it2) == ports.end())
				{
					ports.push_back(*it2);
					server_index[i].push_back(*it2);
				}
			}
		}
		else if (it_server_name != config.end() && it_server == config.end()){
			if (std::find(ports.begin(), ports.end(), "80") == ports.end())
			{
				ports.push_back("80");
				server_index[i].push_back("80");
			}
		}
		else
			return (ports);
	}
	return (ports);
}

// Getting local domains and saving them to a map for each server
void Servers::assignLocalDomain(int server_fd){
	std::map<std::string, std::vector<std::string> > config = getKeyValue();
	for (std::map<std::string, std::vector<std::string> >::iterator it_domain = config.begin(); it_domain != config.end(); it_domain++){
		if (it_domain->first.find("server_name") != std::string::npos){
			std::string server_name = it_domain->first;
			std::size_t pos = server_name.find("]");
			server_name = server_name.substr(0, pos + 1);
			std::string domain = server_name + ".listen";
			std::map<std::string, std::vector<std::string> >::iterator it_domain_listen = config.find(domain);
				if (it_domain_listen == config.end()){
					for (std::vector<std::string>::iterator it = it_domain->second.begin(); it != it_domain->second.end(); it++){
					if (std::find(_domain_to_server[server_fd].begin(), _domain_to_server[server_fd].end(), *it) == _domain_to_server[server_fd].end())
						_domain_to_server[server_fd].push_back(*it);
				}
			}
		}
	}
}

// Getting domains and saving them to a map for each server
void Servers::assignDomain(std::string port, int server_fd){
	if (port == "80")
		assignLocalDomain(server_fd);
	std::map<std::string, std::vector<std::string> > config = getKeyValue();
	for (std::map<std::string, std::vector<std::string> >::iterator it = config.begin(); it != config.end(); it++)
	{
		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (*it2 == port){
				std::string server_name = it->first;
				std::size_t pos = server_name.find("]");
				server_name = server_name.substr(0, pos + 1);
				std::string domain = server_name + ".server_name";
				std::map<std::string, std::vector<std::string> >::iterator it_domain = config.find(domain);
				if (it_domain != config.end()){
					for (std::vector<std::string>::iterator it3 = it_domain->second.begin(); it3 != it_domain->second.end(); it3++){
						if (std::find(_domain_to_server[server_fd].begin(), _domain_to_server[server_fd].end(), *it3) == _domain_to_server[server_fd].end())
							_domain_to_server[server_fd].push_back(*it3);
					}
				}
				else{
					if (std::find(_domain_to_server[server_fd].begin(), _domain_to_server[server_fd].end(), "localhost") == _domain_to_server[server_fd].end())
						_domain_to_server[server_fd].push_back("localhost");
				}
			}
		}
	}
}

// Getter for config file
std::map<std::string, std::vector<std::string> > Servers::getKeyValue() const {
	return (this->_keyValues);
}

// Check if port is valid
int Servers::checkSocketPort(std::string port){
	for (std::string::iterator it = port.begin(); it != port.end(); it++)
	{
		if (!std::isdigit(*it) || std::atoi(port.c_str()) < 0 || std::atoi(port.c_str()) > 65535)
		{
			std::cerr << "Invalid port" << std::endl;
			return 1;
		}
	}
	return (0);
}

// Check if ip is valid
int Servers::checkSocket(std::string ip){
	std::string ip_string;
	std::string port_string;
	if (ip.find(":") == std::string::npos)
		return checkSocketPort(ip);
	else{
		std::stringstream ss_ip;
		std::stringstream ss_port;
		ss_ip << ip;
		getline(ss_ip, ip_string, ':');
		ss_ip >> port_string;
		if (checkSocketPort(port_string) == 1)
			return 1;
	}
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ip_string.c_str(), &(sa.sin_addr));
	if (result == 0) {
		std::cerr << "Invalid address" << std::endl;
		return 1;
	}
	else if (result == -1) {
		std::cerr << "Address conversion failed" << std::endl;
		return 1;
	}
	return 0;
}

bool Servers::getRequest(int client_fd, std::string &request){
	
	char buffer[4096];
	request.clear();
	int bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		request.append(buffer, bytes);
	}
	if (bytes == 0)
	{
		return true;
	}
	if (bytes == -1)
	{
		std::cerr << "Recv failed" << std::endl;
		return true;
	}
	return false;
}

size_t Servers::handleResponse(int reqStatus, int server_fd, int new_socket, HttpRequest &parser) {
		std::string response;
		if (reqStatus != 200)
		{
			Listen host_port = getTargetIpAndPort(_ip_to_server[server_fd]);

			DB db = {configDB_.getServers(), configDB_.getRootConfig()};
			Client client(db, host_port, parser, server_fd_to_index[server_fd], reqStatus);
			client.setupResponse();
			response = client.getResponseString();
		}
		
		ssize_t bytes = write(new_socket, response.c_str(), response.size());
		if (bytes == -1) {
			std::cerr << "Write failed with error: " << strerror(errno) << std::endl;
			return 0;
		}
		return 1;
}
