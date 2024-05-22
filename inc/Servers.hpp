/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:30 by alappas           #+#    #+#             */
/*   Updated: 2024/05/22 21:03:17 by alappas          ###   ########.fr       */
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
		int _epoll_fds;
		std::vector<int> _server_fds;
		std::map<int, std::vector<std::string> > _domain_to_server;
		std::map<int, std::string> _ip_to_server;
		std::map<std::string, std::vector<std::string> > _keyValues;
		std::map<int, std::vector<std::string> > server_index;
		std::map<int, int> server_fd_to_index;
		std::map<int, int> client_to_server;
		int	_client_amount;
		std::map<int, t_client> _client_data;
			
	public:
		
		//Constructors
		Servers(ConfigDB &configDB);
		~Servers();
		Servers(const Servers &rhs);
		Servers &operator=(const Servers &rhs);

		ConfigDB &configDB_;

		//Member functions
		int		checkSocket(std::string port);
		int		checkSocketPort(std::string port);
		int		createSocket();
		// void	bindSocket(int port);
		int		bindSocket(std::string port);
		int		listenSocket();
		int		combineFds(int socket_fd);
		void	createEpoll();
		void	createServers();
		void	assignDomain(std::string port, int server_fd);
		void	assignLocalDomain(int server_fd);
		void	initEvents();
		void	deleteClient(int client_fd);
		std::vector<std::string> getPorts();
		std::map<std::string, std::vector<std::string> > getKeyValue() const;
		bool getRequest(int client_fd, std::string &request);
		int setNonBlocking(int fd);

		//Temporal function until we have a completed config file
		void handleIncomingConnection(int server_fd);
		void handleIncomingData(int client_fd);
		void printServerAddress(int server_fd);
		size_t handleResponse(int reqStatus, int server_fd, int new_socket, HttpRequest &parser);
};

#endif