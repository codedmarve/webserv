/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:30 by alappas           #+#    #+#             */
/*   Updated: 2024/04/02 21:56:28 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERS_HPP
# define SERVERS_HPP
#include "AllHeaders.hpp"

class ConfigDB;
class InputArgs;
struct DB;
struct Listen;
class HttpRequest;

class Servers {
	private:
		std::vector<int> _server_fds;
		int _epoll_fds;
		std::map<int, std::vector<std::string> > _domain_to_server;
		std::map<int, std::string> _ip_to_server;
		std::map<std::string, std::vector<std::string> > _keyValues;
		std::map<int, std::vector<std::string> > server_index;
		std::map<int, int> server_fd_to_index;
	public:
		//Temporal member variable until the config file is completed
		// std::map<std::string, std::vector<std::string> > _server_ports;
		
		//Constructors
		Servers(ConfigDB &configDB);
		~Servers();

		ConfigDB &configDB_;

		//Member functions
		int		checkSocket(std::string port);
		int		checkSocketPort(std::string port);
		int		createSocket();
		// void	bindSocket(int port);
		int		bindSocket(std::string port);
		int		listenSocket();
		int		combineFds();
		void	createEpoll();
		void	createServers();
		void	assignDomain(std::string port, int server_fd);
		void	assignLocalDomain(int server_fd);
		void	initEvents();
		std::vector<std::string> getPorts();
		std::map<std::string, std::vector<std::string> > getKeyValue() const;
		bool getRequest(int client_fd, std::string &request);

		//Temporal function until we have a completed config file
		void handleIncomingConnection(int server_fd);
		void printServerAddress(int server_fd);
		size_t handleResponse(int reqStatus, int server_fd, int new_socket, HttpRequest &parser);
};

#endif