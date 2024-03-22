/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:07 by alappas           #+#    #+#             */
/*   Updated: 2024/03/22 12:46:50 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Servers.hpp"
#include "../../inc/HttpRequestParser.hpp"

Servers::Servers(std::map<std::string, std::vector<std::string> > keyValues) : _server_fds(), _keyValues(keyValues){
	createServers();
	initEvents();
}

Servers::~Servers() {
	for (std::vector<int>::iterator it = _server_fds.begin(); it != _server_fds.end(); ++it)
		close(*it);
	close(_epoll_fds);
}

// Create socket
int Servers::createSocket(){
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// fcntl(server_fd, F_SETFL, O_NONBLOCK);
	if (server_fd == -1) {
		std::cerr << "Socket creation failed" << std::endl;
		return (0);
	}
	_server_fds.push_back(server_fd);
	return (1);
}

// Bind socket
// void Servers::bindSocket(int port){
// 	struct sockaddr_in address;
// 	address.sin_family = AF_INET;
// 	address.sin_addr.s_addr = htonl(INADDR_ANY);
// 	address.sin_port = htons(port);
// 	if (bind(_server_fds.back(), (struct sockaddr *)&address, sizeof(address)) == -1) {
// 		std::cerr << "Bind failed" << std::endl;
// 		return;
// 	}
// }

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
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	if (c_ip != NULL)
		address.sin_addr.s_addr = inet_addr(c_ip);
	else
		address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	// address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(port);
	// std::cout << "Binding server to IP: " << ip_string << ", port: " << port << std::endl;
	if (bind(_server_fds.back(), (struct sockaddr *)&address, sizeof(address)) == -1) {
		std::cerr << "Bind failed" << std::endl;
		return (0);
	}
	return (1);
}


// Create epoll instance
void	Servers::createEpoll(){
	int epoll_fd = epoll_create1(0);// We create an epoll instance
	this->_epoll_fds = epoll_fd;
	if (epoll_fd < 0) {
		std::cerr << "Epoll creation failed" << std::endl;
		exit(1);
	}
	
}

int Servers::listenSocket(){
	if (listen(_server_fds.back(), SOMAXCONN) == -1) {
		std::cerr << "Listen failed" << std::endl;
		return (0);
	}
	return (1);
}

int Servers::combineFds(){
	// int flags = fcntl(_server_fds.back(), F_GETFL, 0);// We set the appropriate flags for the server
	int flags = fcntl(_server_fds.back(), F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	if (flags == -1)// || fcntl(_server_fds.back(), F_SETFL, flags | O_NONBLOCK) == -1) 
	{
		std::cerr << "Fcntl failed" << std::endl;
		return (0);
	}
	struct epoll_event event;
	std::memset(&event, 0, sizeof(event));
	event.events = EPOLLIN; //| EPOLLET;
	event.data.fd = _server_fds.back();
	if (epoll_ctl(this->_epoll_fds, EPOLL_CTL_ADD, _server_fds.back(), &event) == -1) {
		std::cerr << "Epoll_ctl failed" << std::endl;
		return (0);
	}
	return (1);
}

// int number = 123;
//     std::stringstream ss;
//     ss << number;
//     std::string str = ss.str();

void Servers::createServers(){
	
	// this->_server_ports["server.listen"].push_back("8001  8003 8004 8005");
	// this->_server_ports["server.listen"].push_back("8082");
	// this->_server_ports["server.listen"].push_back("8082");
	std::cout << "Creating servers" << std::endl;
	std::vector<std::string> ports;
	createEpoll(); //We create an epoll instance
	ports = getPorts();
	// for (int i = 0; i != INT_MAX; i++){//config.find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]") != config.end(); i++){
	// 	ss.str("");
	// 	ss << i;
	// 	std::string server = "server[" + ss.str() + "].listen";
	// 	std::cout << server << std::endl;
	// 	std::map<std::string, std::vector<std::string> >::iterator it_server = config.find(server);
	// 	std::map<std::string, std::vector<std::string> >::iterator it_http = config.find("http." + server);
	// 	if (it_server != config.end() || it_http != config.end()){
	// 		if (it_server != config.end())
	// 			ports_temp = it_server->second;
	// 		else if (it_http != config.end())
	// 			ports_temp = it_http->second;
	// 		for (std::vector<std::string>::iterator it2 = ports_temp.begin(); it2 != ports_temp.end(); it2++){
	// 			if (std::find(ports.begin(), ports.end(), *it2) == ports.end())
	// 				ports.push_back(*it2);
	// 		}
	// 	}
	// 	// if (it != database.getKeyValue().end())
	// 	// 	std::cout << it->first << std::endl;

	// 	// if (database.getKeyValue().find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]") != database.getKeyValue().end())
	// 	// 	std::cout << database.getKeyValue().find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]")->first << std::endl;
	// 	// std::cout << "I am here" << std::endl;
	// 	// std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	// 	else
	// 		break;
	// }
	for (std::vector<std::string>::iterator it2 = ports.begin(); it2 != ports.end(); it2++) {
		if (!checkSocket(*it2)){
			//We create a singular socket
			if (createSocket()){
				// int enable = 1;
				// setsockopt(_server_fds.back(), SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
				// setsockopt(_server_fds.back(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
				// bindSocket(std::atoi(it2->c_str())); //We bind the socket to the port
				if (!bindSocket(*it2) || !listenSocket() || !combineFds())
					_server_fds.pop_back();
				else
				{
					assignDomain(*it2, _server_fds.back());
					std::cout << "Server created on port " << _ip_to_server[_server_fds.back()] << ", server:" << _server_fds.back() << std::endl;
				}
				// listenSocket(_server_fds.back());
				// combineFds(); //We combine the server and epoll fds
				// IpAddressResolver resolver;
				// std::string hostname = 
				// resolver.resolve("127.0.0.1:8001");
				// listen(_server_fds.back(), SOMAXCONN);
				// std::cout << "Server fd: " << _server_fds.back() << std::endl;
			}
		}
	}
	for (std::map<int, std::vector<std::string> >::iterator it = _domain_to_server.begin(); it != _domain_to_server.end(); it++){
		std::cout << "Server fd: " << it->first;
		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++){
			std::cout << " Domain: " << *it2;
		}
		std::cout << std::endl;
	}
}

void Servers::handleIncomingConnection(int server_fd){
	// struct sockaddr_in address;
	// int addrlen = sizeof(address);
	// int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
	// if (new_socket == -1) {
	// 	// std::cerr << "Accept failed" << std::endl;
	// 	return ;
	// }
	struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
	// std::cout << "Argument server_fd: " << server_fd << std::endl;
    char ip[INET_ADDRSTRLEN];
    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
    if (new_socket == -1) {
        // std::cerr << "Accept failed with error: " << strerror(errno) << std::endl;
        return;
    }
	if (inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN) == NULL) {
    	std::cerr << "inet_ntop failed with error: " << strerror(errno) << std::endl;
		close(new_socket);
    	return;
}
    std::cout << "Connection established on IP: " << _ip_to_server[server_fd] << ", server:" << server_fd << std::endl;
	std::string url = getURL(new_socket);
	std::cout << "Initial URL: " << url << std::endl <<std::endl;
	HttpRequestParser parser;
	parser.parseRequest(url);
	std::cout << "Requested URL: " << std::endl;
	parser.printRequest(parser);
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
    ssize_t bytes = write(new_socket, response.c_str(), response.size());
	if (bytes == -1) {
		std::cerr << "Write failed with error: " << strerror(errno) << std::endl;
		return;
	}
    // Close the socket
    if (close(new_socket) == -1)
		std::cerr << "Close failed with error: " << strerror(errno) << std::endl;
}

void Servers::printServerAddress(int server_fd) {
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    if (getsockname(server_fd, (struct sockaddr *)&address, &addrlen) == -1) {
        std::cerr << "Getsockname failed" << std::endl;
        return;
    }

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(address.sin_addr), ip, INET_ADDRSTRLEN);
    std::cout << "Server is listening on IP: " << ip << ", Port: " << ntohs(address.sin_port) << std::endl;
}

void Servers::initEvents(){
	while (true){
		try{
		// for (std::vector<int>::iterator it = _epoll_fds.begin(); it != _epoll_fds.end(); ++it){
			struct epoll_event events[_server_fds.size()];
			int n = epoll_wait(this->_epoll_fds, events, _server_fds.size(), -1);
			if (n == -1) {
				std::cerr << "Epoll_wait failed" << std::endl;
				return ;
			}
			for (int i = 0; i < n; i++) {
				for (std::vector<int>::iterator it2 = _server_fds.begin(); it2 != _server_fds.end(); ++it2) {
					if (events[i].data.fd == *it2) {
						// std::cout << "Incoming connection" << std::endl;
						// printServerAddress(*it2);
						std::cout << "Incoming connection on server: " << *it2 << std::endl;
						handleIncomingConnection(*it2);
					}
				}
			}
		} catch (std::exception &e){
			std::cerr << e.what() << std::endl;
		}
	}
}

std::vector<std::string> Servers::getPorts(){
	
	std::map<std::string, std::vector<std::string> > config = getKeyValue();
	std::stringstream ss;
	std::vector<std::string> ports_temp;
	std::vector<std::string> ports;

	for (int i = 0; i != INT_MAX; i++){//config.find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]") != config.end(); i++){
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
					ports.push_back(*it2);
			}
		}
		else if (it_server_name != config.end() && it_server == config.end()){
			// std::cerr << "No listen directive found for server[" << i << "], setting port to 80" << std::endl;
			if (std::find(ports.begin(), ports.end(), "80") == ports.end())
				ports.push_back("80");
		}
		// if (it != database.getKeyValue().end())
		// 	std::cout << it->first << std::endl;

		// if (database.getKeyValue().find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]") != database.getKeyValue().end())
		// 	std::cout << database.getKeyValue().find("server[" + static_cast<std::ostringstream*>(&(ss << i))->str() + "]")->first << std::endl;
		// std::cout << "I am here" << std::endl;
		// std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		else
			return (ports);
	}
	return (ports);
}

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
				// std::size_t pos = server_name.find("server[");
				std::size_t pos = server_name.find("]");
				server_name = server_name.substr(0, pos + 1);
				std::string domain = server_name + ".server_name";
				// std::cout << "Domain: " << domain << std::endl;
				std::map<std::string, std::vector<std::string> >::iterator it_domain = config.find(domain);
				// std::cout << "Domain: " << *it_domain->second.begin() << std::endl;
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

// std::vector<std::string> Servers::getServerPort(std::string listen) const {
// 	return (this->_server_ports.at(listen));
// }

std::map<std::string, std::vector<std::string> > Servers::getKeyValue() const {
	return (this->_keyValues);
}

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
		// std::cout << "IP: " << ip_string << " Port: " << port_string << std::endl;
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

std::string Servers::getURL(int client_fd){
	
	std::string url;
	char buffer[2048];
	ssize_t bytes;
	while ((bytes = recv(client_fd, buffer, 2048, 0)) > 0){
		if (bytes == -1){
			std::cerr << "Read failed" << std::endl;
			return ("");
		}
		url += buffer;
		if (bytes < 2048)
			break;
	}
	return (url);
}

