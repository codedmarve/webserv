/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiClient.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 01:56:20 by alappas           #+#    #+#             */
/*   Updated: 2024/05/29 20:01:19 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

CgiClient::CgiClient(Client client, int epoll_fd)
: client_(client), cgiHeadersParsed_(false), cgiRead_(false), cgi_bytes_read_(0), cgi_times_read_(0), status_code_(0),
epoll_fd_(epoll_fd)
{
    cgi_ext_ = client_.getResponse()->getFile()->getMimeExt();
    response_ = &client_.getResponseRef();
    config_ = &client_.getConfigRef();
	cgiHeadersMap_ = &response_->getHeadersRef();
	response_->setConfig(*config_);
	config_->setClient(client_);
    cgi_ = new CgiHandle(config_, cgi_ext_, epoll_fd_);
	if (cgi_->getExitStatus() == 500)
		response_->setStatusCode(500);
    pid_ = cgi_->getPid();
	body_ = &response_->getBody();
	req_body_ = &config_->getBody();
	setCgiPipe(*cgi_);
	toCgi(*cgi_, *req_body_);
}

CgiClient::~CgiClient()
{
	deleteChild(cgi_->getPipeOut());
	closeParentCgiPipe(*cgi_);
	close(cgi_->getPipeOut());
    kill(pid_, SIGKILL);
    delete cgi_;
}

void CgiClient::HandleCgi()
{
	fromCgi(*cgi_);
    if (status_code_ == 0)
        return ;
	else if (cgi_->getExitStatus() == 500)
		status_code_ = 500;
	response_->setStatusCode(status_code_);
	setContentLength();
}

void CgiClient::toCgi(CgiHandle &cgi, std::string &req_body)
{
	if (cgi.getContentLength() > 0)
	{
		int bytesWritten = write(cgi.getPipeIn(), req_body.c_str(), req_body.length());
		if (bytesWritten >= 0)
		{
			req_body = req_body.substr(bytesWritten);
			cgi.deductContentLength(bytesWritten);
			if (cgi.getContentLength() == 0)
				close(cgi.getPipeIn());
		}
		if (bytesWritten == -1)
		{
			status_code_ = 500;
			response_->setStatusCode(500);
		}
	}
}

void CgiClient::fromCgi(CgiHandle &cgi)
{
	int bytesRead;
	char buffer[4096];
	// fd_set readfds;
	// FD_ZERO(&readfds);
	// FD_SET(cgi.getPipeOut(), &readfds);

	// struct timeval tv;
	// tv.tv_sec = 1;
	// tv.tv_usec = 0;
	// int select_value = select(cgi.getPipeOut() + 1, &readfds, NULL, NULL, &tv);
	// if (select_value > 0)
	// 	{
		if ((bytesRead = read(cgi.getPipeOut(), buffer, sizeof(buffer))) > 0)
		{
			body_->append(buffer, bytesRead);
			cgi_bytes_read_ += bytesRead;
			if ((body_->find("\r\n\r\n") != std::string::npos || body_->find("\r\n") != std::string::npos) && !cgiHeadersParsed_)
				handleCgiHeaders(*body_);
			cgiRead_ = true;
		}
		else if (bytesRead == -1 || bytesRead == 0)
		{
			status_code_ = (cgiRead_) ? 200 : 500;
		}
	// }
	// else if (select_value == -1)
	// 	status_code_ = 500;
	// else
	// 	status_code_ = (cgiRead_) ? 200 : 500;
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
			(*cgiHeadersMap_)[key] = value;
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
			body_ = body_.substr(pos + 5);
		else if ((pos = body_.find("\r\n\r\n")) != std::string::npos)
			body_ = body_.substr(pos + 4);
		else if ((pos = body_.find("\r\n\n")) != std::string::npos)
			body_ = body_.substr(pos + 3);
		else if ((pos = body_.find("\r\n")) != std::string::npos)
			body_ = body_.substr(pos + 2);
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

void CgiClient::deleteChild(int child_fd)
{
	if (epoll_ctl(this->epoll_fd_, EPOLL_CTL_DEL, child_fd, NULL) == -1) {
		std::cerr << "Failed to remove client file descriptor from epoll instance." << std::endl;
	}
}