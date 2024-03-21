/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Servers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/11 16:28:30 by alappas           #+#    #+#             */
/*   Updated: 2024/03/17 20:30:04 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERS_HPP
# define SERVERS_HPP
#include "AllHeaders.hpp"

class Servers {
	private:
		std::vector<int> _server_fds;
		int _epoll_fds;
		std::map<int, std::vector<std::string> > _domain_to_server;
		std::map<int, std::string> _ip_to_server;
		std::map<std::string, std::vector<std::string> > _keyValues;
	public:
		//Temporal member variable until the config file is completed
		// std::map<std::string, std::vector<std::string> > _server_ports;
		
		//Constructors
		Servers(std::map<std::string, std::vector<std::string> > _keyValues);
		~Servers();

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
		std::string getURL(int client_fd);

		//Temporal function until we have a completed config file
		void handleIncomingConnection(int server_fd);
		void printServerAddress(int server_fd);
};

#endif