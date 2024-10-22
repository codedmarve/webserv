/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 01:28:32 by alappas           #+#    #+#             */
/*   Updated: 2024/05/24 19:50:21 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLE_HPP
# define CGIHANDLE_HPP

#include "./AllHeaders.hpp"

class RequestConfig;

class CgiHandle{
	private:
		std::map<std::string, std::string>	_env;
		RequestConfig	*_config;
		std::string	_cgi_path;
		pid_t		_cgi_pid;
		std::string	_cgi_ext;
		int			_exit_status;
		char		**_argv;
		char		**_envp;
		char 		*_path;
		int			pipe_in[2];
		int			pipe_out[2];
		int			content_length;
		int			epoll_fd_;
		
	public:
		CgiHandle();
		CgiHandle(RequestConfig *config, std::string cgi_ext, int epoll_fd);
		~CgiHandle();
		CgiHandle(const CgiHandle &other);
		CgiHandle &operator=(const CgiHandle &other);
		void    initEnv();
		void	createEnvArray();
		void	execCgi();
		int		setPipe();
		void	closePipe();
		std::string getIp();
		int getPipeIn();
		int getPipeOut();
		int getExitStatus();
		std::string getExecPath();
		int getContentLength();
		int getPid();
		void setPath();
		void setArgv();
		void deductContentLength(int length);
		std::string	checkShebang();
		int combineFds(int pipe_out);
};

#endif