#include "../../inc/HttpResponse.hpp"

HttpResponse::HttpResponse(RequestConfig &config, int error_code) : config_(config), error_code_(error_code)
{
	status_code_ = 0;
	total_sent_ = 0;
	header_size_ = 0;
	body_size_ = 0;
	redirect_code_ = 0;
	redirect_ = false;
	charset_ = "";
	cgiHeadersParsed_ = false;
	cgiRead = false;
	initMethods();
}

HttpResponse::~HttpResponse() {}

void HttpResponse::cleanUp()
{
	error_code_ = 0;
	status_code_ = 0;
	total_sent_ = 0;
	header_size_ = 0;
	body_size_ = 0;
	redirect_ = false;
	cgiHeadersParsed_ = false;
	cgiRead = false;
	response_.clear();
	body_.clear();
	headers_.clear();
	if (file_)
	{
		delete file_;
		file_ = NULL;
	}
}

bool HttpResponse::shouldDisconnect()
{
	return headers_.find("Connection") != headers_.end() && headers_["Connection"] == "close";
}

void HttpResponse::printMethodMap()
{
	std::map<std::string, int (HttpResponse::*)()>::iterator it;

	for (it = methods_.begin(); it != methods_.end(); ++it)
	{
		std::cout << "Method: " << it->first << ", Function Pointer: " << it->second << std::endl;
	}
}

void HttpResponse::initMethods()
{
	methods_["GET"] = &HttpResponse::GET;
	methods_["HEAD"] = &HttpResponse::GET;
	methods_["POST"] = &HttpResponse::POST;
	methods_["PUT"] = &HttpResponse::PUT;
	methods_["DELETE"] = &HttpResponse::DELETE;
}

void HttpResponse::buildDebugger(std::string method)
{
	std::cout << "Method: " << method << std::endl;
	std::cout << "file path: " << file_->getFilePath() << std::endl;
	std::cout << "Error code: " << error_code_ << std::endl;
	std::cout << "Accepted: " << config_.isMethodAccepted(method) << std::endl;
	std::cout << "MethodList: " << buildMethodList() << std::endl;
	std::cout << "Max body size: " << config_.getClientMaxBodySize() << std::endl;
	std::cout << "Body size: " << config_.getBody().length() << std::endl;
	std::cout << "Auth: " << config_.getAuth() << std::endl;
	std::cout << "CheckAuth: " << checkAuth() << std::endl;
	std::cout << "HandleMethods: " << handleMethods() << std::endl;
	std::cout << "BuildErrorPage: " << buildErrorPage(status_code_) << std::endl;
}

std::pair<std::string, int> HttpResponse::findLocation(std::string target)
{
	std::map<std::string, int> locationsMap = config_.getLocationsMap();
	for (std::map<std::string, int>::iterator it = locationsMap.begin(); it != locationsMap.end(); ++it)
	{
		if (target == it->first)
			return *it;
	}
	return std::make_pair("", 0);
}

void HttpResponse::build()
{
	if (config_.getLociMatched() == 404)
		error_code_ = 404;
	std::string &method = config_.getMethod();
	file_ = new File();

	if (findLocation(config_.getTarget()).first != "")
	{
		config_.setTarget("/");
		config_.setUri("/");
	}

	file_->set_path(config_.getRoot() + "/" + config_.getTarget());

	bool isAuthorized = config_.getAuth() != "off" && !checkAuth();

  if (error_code_ > 200) {

    status_code_ = error_code_;
  }
  else if (!config_.isMethodAccepted(method))
  {
    status_code_ = 405;
    headers_["Allow"] = buildMethodList();
  }
  else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize())
  {
    status_code_ = 413;
  }
  else if (isAuthorized)
    status_code_ = 401;
  else
    status_code_ = handleMethods();
  

	if (status_code_ >= 300 && !body_.length())
		status_code_ = buildErrorPage(status_code_);
	if (!getRedirect())
	{
		createResponse();
	}
}

int HttpResponse::handleMethods()
{
	std::string &method = config_.getMethod();

	if (method == "GET" || method == "HEAD")
	{
		if (file_->is_directory())
		{
			// std::cout << "Handling directory request\n";
			int ret = handleDirectoryRequest();
			if (ret == 200 || ret == 404)
				return ret;
		}
		if (!file_->is_directory())
		{
			// std::cout << "Handling file request\n";
			int ret = handleFileRequest();
			if (ret == 403 || ret == 404)
				return ret;
		}
	}

	if (isCgi(file_->getMimeExt())) {
		std::cout << "I think I am a cgi!\n";
		HandleCgi();
		return status_code_;
	}

	if (method == "PUT" || method == "POST")
		handlePutPostRequest();

	return (this->*(HttpResponse::methods_[method]))();
}

int HttpResponse::handleDirectoryRequest()
{
	std::vector<std::string> indexes = config_.getIndexes();
	std::string index = file_->find_index(indexes);
	std::string newPath;

	if (!index.empty())
	{
		redirect_ = true;
		newPath = "/" + config_.getTarget();
		newPath += "/" + index;
		redirect_target_ = removeDupSlashes(newPath);

		return 200;
	}

	return (config_.getAutoIndex()) ? 0 : 404;
}

int HttpResponse::handleFileRequest()
{
	std::string path = file_->getFilePath();

	if (!file_->exists())
		return 404;

	file_->findMatchingFiles();
	std::vector<std::string> &matches = file_->getMatches();
	handleAcceptLanguage(matches);
	handleAcceptCharset(matches);

	if (!file_->openFile())
		return 403;

	headers_["Last-Modified"] = file_->last_modified();

	return 0;
}

void HttpResponse::handlePutPostRequest()
{
	std::string path = config_.getUri() + "/" + config_.getTarget();

	if (!config_.getUpload().empty())
	{
		File dir(config_.getRoot() + "/" + config_.getUpload());

		path = config_.getUri() + "/" + config_.getUpload() + "/" + config_.getTarget();
		if (dir.exists() && !dir.is_directory())
		{
			dir.deleteFile();
		}

		if (!dir.exists())
		{
			if (mkdir(dir.getFilePath().c_str(), 0755) == -1)
			{
				std::cout << "mkdir : " << strerror(errno) << std::endl;
				// return 500;
			}
		}
		file_->set_path(dir.getFilePath() + "/" + config_.getTarget());
	}
	headers_["Location"] = removeDupSlashes(path);

	// return 201; // Created
}

void HttpResponse::handleAcceptLanguage(std::vector<std::string> &matches)
{
	std::string path = file_->getFilePath();

	if (!config_.getHeader("Accept-Language").empty())
	{
		if (localization(matches))
			file_->set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front(), true);
	}
}

void HttpResponse::handleAcceptCharset(std::vector<std::string> &matches)
{
	std::string path = file_->getFilePath();

	if (!config_.getHeader("Accept-Charset").empty())
	{
		charset_ = accept_charset(matches);
		file_->set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front(), true);
	}
}

int HttpResponse::handleOtherMethods()
{
	std::cerr << "Method not implemented" << std::endl;
	return 405;
}

bool HttpResponse::checkAuth()
{
	std::string authCredentials = config_.getHeader("Authorization");
	if (authCredentials.empty())
		return false;
	std::string token = b64decode(authCredentials.substr(authCredentials.find(' ') + 1));
	return (token == config_.getAuth());
}

std::string HttpResponse::buildMethodList()
{
	std::string list;
	std::vector<std::string> methods = config_.getMethods();

	std::vector<std::string>::iterator it = methods.begin();

	while (it != methods.end())
	{
		list += *it;
		it++;
		if (it != methods.end())
			list += ", ";
	}
	return list;
}

std::string HttpResponse::response_log(LogLevel level)
{
	std::string ret;

	if (level == INFO)
	{
		ret = "[status: " + ftos(status_code_) + " " + file_->getStatusCode(status_code_) + "]";
		if (headers_.count("Content-Length"))
			ret = ret + " [length: " + headers_["Content-Length"] + "]";
	}
	else if (level > INFO)
	{
		ret = "\n\n" + response_.substr(0, header_size_ - 4) + "\n";
		if (body_size_)
		{
			if (body_size_ < 200)
				ret = ret + "\n" + response_.substr(header_size_);
			else
				ret = ret + "\n" + response_.substr(header_size_, 200) + "...";
		}
	}

	// Write the log to stdout
	std::cout << ret << std::endl;

	return ret; // Optionally return the log string if needed elsewhere
}

bool HttpResponse::getRedirect()
{
	return redirect_;
}

std::string HttpResponse::redirect_target()
{
	return redirect_target_;
}

void HttpResponse::createResponse()
{
	headers_["Server"] = "webserv/1.1";

	if (config_.getMethod() == "HEAD")
	{
		body_.clear();
	}

	if (status_code_ < 400 && redirect_code_)
	{
		status_code_ = redirect_code_;
	}

	std::string status_code_phrase = file_->getStatusCode(status_code_);
	std::string status_line = "HTTP/1.1 " + ftos(status_code_) + " " + status_code_phrase + "\r\n";

	headers_["Date"] = get_http_date();

	std::string header_block;
	for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
	{
		header_block += it->first + ": " + it->second + "\r\n";
	}

	header_block += "\r\n"; // add empty line after headers

	// Concatenate status line, headers, and body
	response_ = status_line + header_block + body_;

	// Calculate header and body sizes
	header_size_ = status_line.size() + header_block.size();
	body_size_ = body_.size();
	body_.clear();
	// std::cout << "RESPONSE: " << response_ << std::endl;
}

std::string HttpResponse::getSampleResponse()
{
	return response_;
}

int HttpResponse::sendResponse(int fd)
{
	std::string fullResponse = response_;
	if (!body_.empty())
	{
		headers_["Content-Length"] = ftos(body_.length());
		fullResponse.append("\r\n\r\n");
		fullResponse.append(body_);

	}

	int ret = send(fd, fullResponse.c_str() + total_sent_, fullResponse.length() - total_sent_, 0);
	if (ret <= 0)
	{
		std::cerr << "send : " << strerror(errno) << std::endl;
		return -1;
	}

	total_sent_ += ret;
	if (total_sent_ >= fullResponse.length())
		return 0;

	return 1;
}

bool HttpResponse::isCgi(std::string ext)
{
	VecStr &cgi = config_.getCgi();
	return std::find(cgi.begin(), cgi.end(), ext) != cgi.end();
}

void HttpResponse::HandleCgi()
{
	CgiHandle cgi(&config_, file_->getMimeExt());
	cgi.execCgi();
	printf("CGI exit status: %d\n", cgi.getExitStatus());
	// std::cout << "I am here" << std::endl;
	pid_t exit_status = waitpid(cgi.getPid(), &status_code_, WNOHANG);
	if ((status_code_ = cgi.getExitStatus()) == 500 || exit_status != 0)
	{
		status_code_ = 500;
		closeParentCgiPipe(cgi);
		return ;
	}
	setCgiPipe(cgi);
	std::string req_body = config_.getBody();
	while (status_code_ == 0)
	{
		// std::cout << "STATUS: " << status_code_ << std::endl;
		toCgi(cgi, req_body);
		fromCgi(cgi);
	}
	waitpid(cgi.getPid(), &exit_status, 0);
	closeParentCgiPipe(cgi);
	if (exit_status == 256)
		status_code_ = 500;
	std::cout << "STATUS: " << status_code_ << std::endl;
}

void HttpResponse::toCgi(CgiHandle &cgi, std::string &req_body)
{
	if (cgi.getContentLength() > 0)
	{
		std::string body = req_body;
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
			std::cerr << "write : " << strerror(errno) << std::endl;
			status_code_ = 500;
		}
	}
}

void HttpResponse::fromCgi(CgiHandle &cgi)
{
	int bytesRead;
	char buffer[4096];
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(cgi.getPipeOut(), &readfds);

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if (select(cgi.getPipeOut() + 1, &readfds, NULL, NULL, &tv) > 0)
	{
		if ((bytesRead = read(cgi.getPipeOut(), buffer, sizeof(buffer))) > 0)
		{
			body_.append(buffer, bytesRead);
			if (body_.find("\r\n\r\n") != std::string::npos && !cgiHeadersParsed_)
				handleCgiHeaders(body_);
			cgiRead = true;
		}
		else if (bytesRead == -1 || bytesRead == 0)
		{
			std::cout << "bytesRead: " << bytesRead << std::endl;
			status_code_ = (cgiRead) ? 200 : 500;
			return ;
		}
	}
	else if (select(cgi.getPipeOut() + 1, &readfds, NULL, NULL, &tv) == -1)
	{
		std::cerr << "Error reading CGI response" << std::endl;
		status_code_ = 500;
	}
	else
		status_code_ = 200;
}

void HttpResponse::setCgiPipe(CgiHandle &cgi)
{
	int flags;
	if ((flags = fcntl(cgi.getPipeOut(), F_GETFL, 0)) == -1 || fcntl(cgi.getPipeOut(), F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl error" << std::endl;
		status_code_ = 500;
		return;
	}
}

void HttpResponse::parseCgiHeaders()
{
	std::vector<std::string> headerLines = split(cgiHeaders_, '\n');
	std::string key;
	std::string value;
	size_t 		pos;
	std::string header;
	for (size_t i = 0; i < headerLines.size(); i++)
	{
		header = headerLines[i];
		pos = header.find(":");
		if (pos != std::string::npos && pos <= header.length() - 2)
		{
			key = header.substr(0, header.find(":"));
			value = header.substr(header.find(":") + 2);
			headers_[key] = value;
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
	for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
	{
		std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
	}
}

void HttpResponse::handleCgiHeaders(std::string &body_)
{
	std::string::size_type pos = body_.find("\r\n\r\n");
	if (pos != std::string::npos)
	{
		cgiHeaders_ = body_.substr(0, pos);
		if (body_.find("\r\n\r\n\n") != std::string::npos)
			body_ = body_.substr(pos + 5);
		else
			body_ = body_.substr(pos + 4);
		cgiHeadersParsed_ = true;
		parseCgiHeaders();
	}
	else
	{
		cgiHeaders_ = body_;
		body_.clear();
	}
	// std::vector<std::string> headerLines = split(headers, "\r\n");
	// for (size_t i = 0; i < headerLines.size(); i++)
	// {
	// 	std::string header = headerLines[i];
	// 	std::string key = header.substr(0, header.find(":"));
	// 	std::string value = header.substr(header.find(":") + 2);
	// 	headers_[key] = value;
	// }
}

void HttpResponse::closeParentCgiPipe(CgiHandle &cgi)
{
	if (cgi.getPipeIn() != -1)
		close(cgi.getPipeIn());
	if (cgi.getPipeOut() != -1)
		close(cgi.getPipeOut());
}