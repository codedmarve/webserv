/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 01:28:35 by alappas           #+#    #+#             */
/*   Updated: 2024/03/22 12:46:31 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/CgiHandle.hpp"

CgiHandle::CgiHandle()
: _cgi_path(NULL), _cgi_pid(-1), _exit_status(0){}

CgiHandle::~CgiHandle(){
	delete [] this->_argv;
	delete [] this->_envp;
}

CgiHandle::CgiHandle(const CgiHandle &other)
: _env(other._env), _cgi_path(other._cgi_path), _cgi_pid(other._cgi_pid),
	_exit_status(other._exit_status){}

CgiHandle& CgiHandle::operator=(const CgiHandle &other){
	this->_env = other._env;
	this->_cgi_path = other._cgi_path;
	this->_cgi_pid = other._cgi_pid;
	this->_exit_status = other._exit_status;
	return *this;
}

void CgiHandle::initEnv(){

	std::string cgiExec = "/cgi-bin/hello.sh";
	
	this->_env["CONTENT_TYPE"] = "";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["REMOTE_ADDR"] = "";
	this->_env["QUERY_STRING"] = "";
	this->_env["REMOTE_PORT"] = "";
	this->_env["CONTENT_LENGTH"] = "";
	this->_env["REQUEST_URI"] = "";
	this->_env["SERVER_SOFTWARE"] = "AMANIX";
	this->_env["PATH"] = "";
	this->_env["HTTP_REFERER"] = "";
	this->_env["SERVER_PROTOCOL"] = "";
	this->_env["PATH_INFO"] = "";
	this->_env["REQUEST_METHOD"] = "";
	this->_env["PWD"] = "";
	this->_env["SERVER_PORT"] = "8002";
	this->_env["SCRIPT_NAME"] = "";
	this->_env["REMOTE_USER"] = "";
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_NAME"] = "localhost";//getHeader?
}

void CgiHandle::createEnvArray(){
	std::map<std::string, std::string>::iterator it;
	int i = 0;
	this->_argv = new char*[this->_env.size() + 1];
	for (it = this->_env.begin(); it != this->_env.end(); it++)
	{
		this->_argv[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	this->_argv[i] = NULL;

}

void CgiHandle::execCgi(){
	
	std::string cgiExec = "/cgi-bin/hello.sh";
	this->_cgi_path = getenv("PWD") + cgiExec;
	
	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		std::cerr << "Error: invalid arguments" << std::endl;
		this->_exit_status = 500;
		return ;
	}
	if (this->setPipe() == -1){
		this->_exit_status = 500;
		return ;
	}
	this->_cgi_pid = fork();
	if (_cgi_pid < 0)
	{
		std::cerr << "Error: fork failed" << std::endl;
		this->_exit_status = 500;
		return ;
	}
	else
	{
		dup2(this->pipe_in[0], STDIN_FILENO);
		dup2(this->pipe_out[1], STDOUT_FILENO);
		closePipe();
		_exit_status = execve(this->_cgi_path.c_str(), this->_argv, this->_envp);
		exit(_exit_status);
	}
}

int CgiHandle::setPipe(){
	if (pipe(this->pipe_in) == -1 || pipe(this->pipe_out) == -1)
	{
		std::cerr << "Error: pipe failed" << std::endl;
		return (-1);
	}
	return (0);
}

void CgiHandle::closePipe(){
	close(this->pipe_in[0]);
	close(this->pipe_in[1]);
	close(this->pipe_out[0]);
	close(this->pipe_out[1]);
}