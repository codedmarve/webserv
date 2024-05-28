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
	cgiRead_ = false;
	cgi_true_ = false;
	cgi_bytes_read_ = 0;
	cgi_times_read_ = 0;
	initMethods();
}

HttpResponse::~HttpResponse()
{
	cleanUp();
}

HttpResponse::HttpResponse(const HttpResponse &rhs)
	: config_(rhs.config_), file_(rhs.file_), error_code_(rhs.error_code_),
	  worker_id_(rhs.worker_id_), total_sent_(rhs.total_sent_),
	  status_code_(rhs.status_code_), response_(rhs.response_),
	  body_(rhs.body_), redirect_(rhs.redirect_),
	  redirect_target_(rhs.redirect_target_),
	  redirect_code_(rhs.redirect_code_), header_size_(rhs.header_size_),
	  body_size_(rhs.body_size_), charset_(rhs.charset_),
	  methods_(rhs.methods_), headers_(rhs.headers_),
	  cgiHeaders_(rhs.cgiHeaders_), cgiHeadersParsed_(rhs.cgiHeadersParsed_),
	  cgiRead_(rhs.cgiRead_), cgi_bytes_read_(rhs.cgi_bytes_read_),
	  cgi_times_read_(rhs.cgi_times_read_), cgi_true_(rhs.cgi_true_)
{
	file_ = (rhs.file_) ? new File(*rhs.file_) : NULL;
}

HttpResponse::HttpResponse(const HttpResponse &rhs, RequestConfig &config)
	: config_(config), file_(rhs.file_), error_code_(rhs.error_code_),
	  worker_id_(rhs.worker_id_), total_sent_(rhs.total_sent_),
	  status_code_(rhs.status_code_), response_(rhs.response_),
	  body_(rhs.body_), redirect_(rhs.redirect_),
	  redirect_target_(rhs.redirect_target_),
	  redirect_code_(rhs.redirect_code_), header_size_(rhs.header_size_),
	  body_size_(rhs.body_size_), charset_(rhs.charset_),
	  methods_(rhs.methods_), headers_(rhs.headers_),
	  cgiHeaders_(rhs.cgiHeaders_), cgiHeadersParsed_(rhs.cgiHeadersParsed_),
	  cgiRead_(rhs.cgiRead_), cgi_bytes_read_(rhs.cgi_bytes_read_),
	  cgi_times_read_(rhs.cgi_times_read_), cgi_true_(rhs.cgi_true_)
{
	file_ = (rhs.file_) ? new File(*rhs.file_) : NULL;
}

HttpResponse &HttpResponse::operator=(const HttpResponse &rhs)
{
	if (this != &rhs)
	{
		cleanUp();

		config_ = rhs.config_;
		error_code_ = rhs.error_code_;
		worker_id_ = rhs.worker_id_;
		total_sent_ = rhs.total_sent_;
		status_code_ = rhs.status_code_;
		response_ = rhs.response_;
		body_ = rhs.body_;
		redirect_ = rhs.redirect_;
		redirect_target_ = rhs.redirect_target_;
		redirect_code_ = rhs.redirect_code_;
		header_size_ = rhs.header_size_;
		body_size_ = rhs.body_size_;
		charset_ = rhs.charset_;
		methods_ = rhs.methods_;
		headers_ = rhs.headers_;
		cgiHeaders_ = rhs.cgiHeaders_;
		cgiHeadersParsed_ = rhs.cgiHeadersParsed_;
		cgiRead_ = rhs.cgiRead_;
		cgi_bytes_read_ = rhs.cgi_bytes_read_;
		cgi_times_read_ = rhs.cgi_times_read_;
		cgi_true_ = rhs.cgi_true_;

		file_ = (rhs.file_) ? new File(*rhs.file_) : NULL;
	}
	return *this;
}

void HttpResponse::cleanUp()
{
	error_code_ = 0;
	status_code_ = 0;
	total_sent_ = 0;
	header_size_ = 0;
	body_size_ = 0;
	redirect_ = false;
	cgiHeadersParsed_ = false;
	cgiRead_ = false;
	cgi_bytes_read_ = 0;
	response_.clear();
	body_.clear();
	headers_.clear();

	if (file_)
	{
		delete file_;
		file_ = NULL;
	}
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

	if (error_code_ > 200)
	{
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
	if (!getRedirect() && !cgi_true_)
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
			int ret = handleDirectoryRequest();
			if (ret == 200 || ret == 404)
				return ret;
		}
		if (!file_->is_directory())
		{
			int ret = handleFileRequest();
			if (ret == 403 || ret == 404)
				return ret;
		}
	}

	if (isCgi(file_->getMimeExt()))
	{
		cgi_true_ = true;
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
	foundIndex = !index.empty();

	if (foundIndex)
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
	if (config_.getRedirCode())
	{
		redirect_code_ = config_.getRedirCode();

		if (redirect_code_ == 301)
		{
			std::map<int, std::string> m = config_.getRedirectionMap();
			headers_["Location"] = m[redirect_code_];
		}
	}

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

	std::cout << status_code_ << " " << ((status_code_ < 400) ? BGREEN : BRED) << status_code_phrase << RESET << std::endl;

	if (!foundIndex && config_.getAutoIndex())
		std::cout << CURSIVE_GRAY << "Displaying AutoIndex!!!" << RESET << std::endl;
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

void HttpResponse::setStatusCode(int code)
{
	status_code_ = code;
}

void HttpResponse::setHeader(std::string key, std::string value)
{
	headers_[key] = value;
}

void HttpResponse::appendBody(char *buffer, int size)
{
	body_.append(buffer, size);
}

std::string &HttpResponse::getBody()
{
	return body_;
}

File *HttpResponse::getFile()
{
	return file_;
}

void HttpResponse::setBody(std::string body)
{
	body_ = body;
}

std::map<std::string, std::string> HttpResponse::getHeaders()
{
	return headers_;
}

void HttpResponse::setSubstr(int start, std::size_t end)
{
	body_ = body_.substr(start, end);
}

void HttpResponse::setSubstr(int start)
{
	body_ = body_.substr(start);
}

void HttpResponse::clearBody()
{
	body_.clear();
}

void HttpResponse::setConfig(RequestConfig &config)
{
	config_ = config;
}

std::map<std::string, std::string> &HttpResponse::getHeadersRef()
{
	return headers_;
}

bool HttpResponse::getCgiStatus()
{
	return cgi_true_;
}