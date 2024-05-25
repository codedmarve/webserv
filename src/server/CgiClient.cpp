/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 01:56:20 by alappas           #+#    #+#             */
/*   Updated: 2024/05/25 22:32:17 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//Check private variables!!!!!!!!!!!!!!!!!!!

#include "../../inc/AllHeaders.hpp"

CgiClient::CgiClient(Client client, int epoll_fd)
: client_(client), cgiHeadersParsed_(false), cgiRead_(false), cgi_bytes_read_(0), cgi_times_read_(0), status_code_(0),
epoll_fd_(epoll_fd)
{
	// std::cout << "CgiClient Constructor\n";
    // client_ = new Client(client);
	// client_ = client;
	// client_ = &client;
	// client_ = client.clone();
    cgi_ext_ = client_.getResponse()->getFile()->getMimeExt();
    response_ = &client_.getResponseRef();
    config_ = &client_.getConfigRef();
	// I wantv a copy of the client
	config_->setClient(client_);
	// response_->setConfig(*config_);
	// std::cout << "The address of the client: " << &client_ << std::endl;
	// std::cout << "The address of the response: " << response_ << std::endl;
	// std::cout << "The address of the config: " << config_ << std::endl << std::endl;

	// std::cout << "The address in the REQUEST: " << &config_->client_ << std::endl << std::endl;
	
	// std::cout << "The address of the response: " << response_ << std::endl;
	// std::cout << "The address of the config: " << config_ << std::endl;
    // cgi_ = CgiHandle(config_, cgi_ext_);
    cgi_ = new CgiHandle(config_, cgi_ext_, epoll_fd_);
	if (cgi_->getExitStatus() == 500)
		response_->setStatusCode(500);
    pid_ = cgi_->getPid();
    std::string req_body = config_->getBody();
	// setCgiPipe(*cgi_);
    toCgi(*cgi_, config_->getBody());
}

CgiClient::~CgiClient()
{
	// std::cout << "CgiClient Destructor\n";
    // delete client_;
    delete cgi_;
	closeParentCgiPipe(*cgi_);
	close(cgi_->getPipeOut());
    kill(pid_, SIGKILL);
	// delete config_;
	// delete response_;
}

void CgiClient::HandleCgi()
{
	fromCgi(*cgi_);
    if (status_code_ == 0)
        return ;
	else if (cgi_->getExitStatus() == 500)
		status_code_ = 500;
	// closeParentCgiPipe(*cgi_);
	// close(cgi_->getPipeOut());
    // kill(pid_, SIGKILL);
	// std::cout << "Response status code: " << status_code_ << std::endl;
	// std::cout << "RESPONSE BODY: " << response_->getBody() << std::endl;
	setContentLength();
	// for (std::map<std::string, std::string>::iterator it = response_->getHeaders().begin(); it != response_->getHeaders().end(); it++)
	// {
	// 	std::cout << "I seg here\n";
	// 	std::cout << "Key!: " << it->first << ", Value!: " << << std::endl;
	// }
	// std::cout << "Content-Length: " << config_->getHeader("Content-Length") << std::endl;
}

void CgiClient::toCgi(CgiHandle &cgi, std::string req_body)
{
	if (cgi.getContentLength() > 0)
	{
		int bytesWritten = write(cgi.getPipeIn(), req_body.c_str(), req_body.length());
		if (bytesWritten >= 0)
		{
			config_->setSubstr(bytesWritten);
			// req_body = req_body.substr(bytesWritten);
			cgi.deductContentLength(bytesWritten);
			if (cgi.getContentLength() == 0)
				close(cgi.getPipeIn());
		}
		if (bytesWritten == -1)
		{
			std::cerr << "write : " << strerror(errno) << std::endl;
			status_code_ = 500;
			response_->setStatusCode(500);
		}
	}
}

void CgiClient::fromCgi(CgiHandle &cgi)
{
	int bytesRead;
	char buffer[4096];
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(cgi.getPipeOut(), &readfds);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	int select_value = select(cgi.getPipeOut() + 1, &readfds, NULL, NULL, &tv);
	if (select_value > 0)
	{
		if ((bytesRead = read(cgi.getPipeOut(), buffer, sizeof(buffer))) > 0)
		{
			response_->appendBody(buffer, bytesRead);
			// std::cout << "BUFFER: " << buffer << std::endl;
			// std::cout << "Config: " << config_->getBody() << std::endl;
			body_ = response_->getBody();
			// std::cout << "BODY TWO: " << body_ << std::endl;
			cgi_bytes_read_ += bytesRead;
			if ((body_.find("\r\n\r\n") != std::string::npos || body_.find("\r\n") != std::string::npos) && !cgiHeadersParsed_)
				handleCgiHeaders(body_);
			cgiRead_ = true;
			// cgi_times_read_++;
			// if (cgi_times_read_ > 200)
			// 	status_code_ = 200;
		}
		else if (bytesRead == -1 || bytesRead == 0)
		{
			std::cout << "bytesRead: " << bytesRead << std::endl;
			status_code_ = (cgiRead_) ? 200 : 500;
			
		}
		if (bytesRead < 4096)
			status_code_ = 200;
	}
	else if (select_value == -1)
		status_code_ = 500;
	else
		status_code_ = (cgiRead_) ? 200 : 500;
	response_->setStatusCode(status_code_);
}


void CgiClient::parseCgiHeaders()
{
	std::vector<std::string> headerLines = split(cgiHeaders_, '\n');
	std::string key;
	std::string value;
	size_t pos;
	std::string header;
	for (size_t i = 0; i < headerLines.size(); i++)
	{
		header = headerLines[i];
		pos = header.find(":");
		if (pos != std::string::npos && pos <= header.length() - 2)
		{
			key = header.substr(0, header.find(":"));
			value = header.substr(header.find(":") + 2);
            response_->setHeader(key, value);
		}
		else if (header.find("HTTP/1.1") != std::string::npos)
		{
			pos = header.find(" ");
			if (pos != std::string::npos && pos <= header.length() - 2)
			{
				std::string status = header.substr(header.find(" ") + 1, 3);
				status_code_ = atoi(status.c_str());
			}
		}
	}
	std::map<std::string, std::string> headers = response_->getHeaders();
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
	{
		std::cout << "Key!: " << it->first << ", Value!: " << it->second << std::endl;
	}
}

void CgiClient::handleCgiHeaders(std::string &body_)
{
	std::string::size_type pos;
	if (body_.find("\r\n\r\n") != std::string::npos)
		pos = body_.find("\r\n\r\n");
	else if (body_.find("\r\n") != std::string::npos)
		pos = body_.find("\r\n");
	if (pos != std::string::npos)
	{
		cgiHeaders_ = body_.substr(0, pos);
		if ((pos = body_.find("\r\n\r\n\n")) != std::string::npos)
			// body_ = body_.substr(pos + 5);
			response_->setBody(body_.substr(pos + 5));
		else if ((pos = body_.find("\r\n\r\n")) != std::string::npos)
			response_->setBody(body_.substr(pos + 4));
			// body_ = body_.substr(pos + 4);
		else if ((pos = body_.find("\r\n\n")) != std::string::npos)
			response_->setBody(body_.substr(pos + 3));
			// body_ = body_.substr(pos + 3);
		else if ((pos = body_.find("\r\n")) != std::string::npos)
			response_->setBody(body_.substr(pos + 2));
			// body_ = body_.substr(pos + 2);
		cgiHeadersParsed_ = true;
		parseCgiHeaders();
	}
	else
	{
		cgiHeaders_ = response_->getBody();
		response_->clearBody();
	}
}
void CgiClient::setCgiPipe(CgiHandle &cgi)
{
	int flags;
	if ((flags = fcntl(cgi.getPipeOut(), F_GETFL, 0)) == -1 || fcntl(cgi.getPipeOut(), F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl error" << std::endl;
		status_code_ = 500;
		return;
	}
}

void CgiClient::closeParentCgiPipe(CgiHandle &cgi)
{
	if (cgi.getPipeIn() != -1)
		close(cgi.getPipeIn());
	if (cgi.getPipeOut() != -1)
		close(cgi.getPipeOut());
}

std::string CgiClient::getResponseString()
{
    return response_->getSampleResponse();
}

HttpResponse &CgiClient::getResponse()
{
    return client_.getResponseRef();
}

int CgiClient::getStatusCode()
{
    return status_code_;
}

void CgiClient::setContentLength(void)
{
	std::stringstream ss;
	ss << cgi_bytes_read_;
	response_->setHeader("Content-Length", ss.str());
}

int CgiClient::getPipeOut(void){
	return (cgi_->getPipeOut());
}
