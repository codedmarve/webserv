/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandle.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 01:28:35 by alappas           #+#    #+#             */
/*   Updated: 2024/05/09 01:34:11 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/Test.hpp"

// CgiHandle::CgiHandle()
// : _cgi_path(NULL), _cgi_pid(-1), _exit_status(0){}

CgiHandle::CgiHandle(RequestConfig *config, std::string cgi_ext)
: _config(config), _cgi_path(""), _cgi_pid(-1), _cgi_ext(cgi_ext), _exit_status(0),
	_argv(NULL), _envp(NULL), _path(NULL), pipe_in(), pipe_out(), content_length(0){
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
	closePipe();
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
	
	if (this->_config->getMethod() == "POST")
	{
		this->_env["CONTENT_TYPE"] = this->_config->getHeader("content-type");
		ss << this->_config->getHeader("content-length");
		std::string content_length = ss.str();
		this->_env["CONTENT_LENGTH"] = content_length;
		this->content_length = atoi(content_length.c_str());
		ss.clear();
	}
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// this->_env["REMOTE_ADDR"] = getIp();
	this->_env["QUERY_STRING"] = this->_config->getQuery();
	ss << this->_config->getPort();
	std::string port = ss.str();
	this->_env["REMOTE_PORT"] = port;
	this->_env["HTTP_REFERER"] = "http://" + this->_config->getHost() + ":" + port + "/cgi-bin" + this->_config->getUri();
	this->_env["SERVER_PORT"] = port;
	ss.clear();
	this->_env["REQUEST_URI"] = this->_config->getUri();
	this->_env["SERVER_SOFTWARE"] = "AMANIX";
	this->_env["PATH"] = std::string (getenv("PATH"));
	this->_env["SERVER_PROTOCOL"] = this->_config->getProtocol();
	this->_env["REQUEST_METHOD"] = this->_config->getMethod();
	this->_env["PWD"] = std::string (getenv("PWD"));
	this->_env["SCRIPT_NAME"] = this->_config->getUri();
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["SERVER_NAME"] = "localhost";//getHeader?
	// this->_env["REMOTE_USER"] = "";
	// this->_env["PATH_INFO"] = "";
}

void CgiHandle::createEnvArray(){
	std::map<std::string, std::string>::iterator it;
	int i = 0;
	this->_envp = new char*[this->_env.size() + 1];
	for (it = this->_env.begin(); it != this->_env.end(); it++)
	{
		this->_envp[i] = strdup((it->first + "=" + it->second).c_str());
		i++;
	}
	this->_envp[i] = NULL;
}

void CgiHandle::execCgi(){
	this->setPath();
	this->setArgv();
	this->createEnvArray();
	this->setPipe();
	if (!this->_path || !this->_argv)// || !this->_envp)
	{
		std::cerr << "Error: execve argument creation failed" << std::endl;
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
	else if (_cgi_pid == 0)
	{
		if (dup2(this->pipe_in[0], STDIN_FILENO) == -1 || dup2(this->pipe_out[1], STDOUT_FILENO) == -1
			|| (_exit_status = execve(this->_argv[0], this->_argv, this->_envp)) == -1)
		{
			std::cerr << "Error: execve failed" << std::endl;
			closePipe();
			exit(500);
		}
		closePipe();
	}
	// waitpid(this->_cgi_pid, &this->_exit_status, 0);
}

int CgiHandle::setPipe(){
	if (pipe(this->pipe_in) == -1 || pipe(this->pipe_out) == -1)
	{
		closePipe();
		std::cerr << "Error: pipe failed" << std::endl;
		return (-1);
	}
	return (0);
}

void CgiHandle::closePipe(){
	if (this->pipe_in[0] != -1)
		close(this->pipe_in[0]);
	if (this->pipe_in[1] != -1)
		close(this->pipe_in[1]);
	if (this->pipe_out[0] != -1)
		close(this->pipe_out[0]);
	if (this->pipe_out[1] != -1)
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
	path += "/" + this->_config->getRoot() + this->_config->getUri();
	this->_path = strdup(path.c_str());
}

std::string CgiHandle::getExecPath(){
	if (checkShebang().size() > 0)
		return checkShebang();
	if (this->_cgi_ext == ".py")
		return "/usr/bin/python3";
	else if (this->_cgi_ext == ".sh")
		return "/bin/bash";
	else
		return "";
}

void CgiHandle::setArgv(){
	this->_argv = new char*[3];
	this->_argv[0] = strdup(this->getExecPath().c_str());
	this->_argv[1] = strdup(this->_path);
	this->_argv[2] = NULL;
}

int CgiHandle::getPipeIn(){
	return this->pipe_in[1];
}

int CgiHandle::getPipeOut(){
	return this->pipe_out[0];
}

int CgiHandle::getExitStatus(){
	return this->_exit_status;
}
int CgiHandle::getContentLength(){
	return this->content_length;
}

int CgiHandle::getPid(){
	return this->_cgi_pid;
}

void CgiHandle::deductContentLength(int length){
	this->content_length -= length;
}

std::string CgiHandle::checkShebang(){
	std::ifstream file(this->_path);
	if (!file.is_open())
	{
		return "";
	}
	std::string line;
	std::getline(file, line);
	if (line.find("#!") == std::string::npos)
		return "";
	else{
		std::stringstream ss(line);
		return(line.substr(line.find("#!") + 2));
	}
}