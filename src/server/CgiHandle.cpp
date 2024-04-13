/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 01:28:35 by alappas           #+#    #+#             */
/*   Updated: 2024/04/10 03:10:13 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Test.hpp"

// CgiHandle::CgiHandle()
// : _cgi_path(NULL), _cgi_pid(-1), _exit_status(0){}

CgiHandle::CgiHandle(RequestConfig *config)
: _config(config), _cgi_path(""), _cgi_pid(-1), _exit_status(0), _argv(NULL), _envp(NULL), _path(NULL){
	this->initEnv();
	
}

CgiHandle::~CgiHandle(){
	if (this->_argv)
	{
		for (int i = 0; this->_argv[i]; i++)
			free(this->_argv[i]);
		delete[] this->_argv;
	}
	if (this->_envp)
	{
		for (int i = 0; this->_envp[i]; i++)
			free(this->_envp[i]);
		delete[] this->_envp;
	}
	if (this->_path)
		free(this->_path);
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

	std::stringstream ss;
	
	this->_env["CONTENT_TYPE"] = this->_config->getHeader("content-type");
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["REMOTE_ADDR"] = getIp();
	this->_env["QUERY_STRING"] = this->_config->getQuery();
	this->_env["REMOTE_PORT"] = this->_config->getPort();
	ss << this->_config->getContentLength();
	this->_env["CONTENT_LENGTH"] = ss.str();
	ss.clear();
	this->_env["REQUEST_URI"] = this->_config->getUri();
	this->_env["SERVER_SOFTWARE"] = "AMANIX";
	this->_env["PATH"] = std::string (getenv("PATH"));
	this->_env["SERVER_PROTOCOL"] = this->_config->getProtocol();
	this->_env["REQUEST_METHOD"] = this->_config->getMethod();
	this->_env["PWD"] = std::string (getenv("PWD"));
	ss << this->_config->getPort();
	this->_env["HTTP_REFERER"] = "http://" + this->_config->getHost() + ":" + ss.str() + this->_config->getUri();
	this->_env["SERVER_PORT"] = ss.str();
	this->_env["SCRIPT_NAME"] = this->_config->getUri();
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_NAME"] = "localhost";//getHeader?
	// this->_env["REMOTE_USER"] = "";
	// this->_env["PATH_INFO"] = "";
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
	
	this->setPath();
	this->setArgv();
	this->createEnvArray();
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
		_exit_status = execve(this->_path, this->_argv, this->_envp);
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

std::string CgiHandle::getIp(){
	std::string full_ip = this->_config->getHost();
	std::stringstream ss;
	std::getline(ss, full_ip, ':');
	return full_ip;
}

void CgiHandle::setPath(){
	std::string path(getenv("PWD"));
	path += this->_config->getUri();
	this->_path = strdup(path.c_str());
}

void CgiHandle::setArgv(){
	this->_argv = new char*[2];
	this->_argv[0] = strdup(this->_path);
	this->_argv[1] = NULL;
}