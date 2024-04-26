#include "../../inc/HttpResponse.hpp"

/// @brief
// How response is built:
// HTTP/1.1 200 OK
// Date: Wed, 13 Apr 2024 12:00:00 GMT
// Server: webserv/1.0
// Content-Type: text/html
// Content-Length: 1234

// <!DOCTYPE html>
// <html>
// <head>
//     <title>Example Page</title>
// </head>
// <body>
//     <h1>Hello, World!</h1>
//     <p>This is an example page.</p>
// </body>
// </html>

HttpResponse::HttpResponse(RequestConfig &config, int error_code) : config_(config), error_code_(error_code)
{
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_code_ = 0;
  redirect_ = false;
  charset_ = "";
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

void HttpResponse::build()
{
  std::string &method = config_.getMethod();
  file_ = new File();

  file_->set_path(config_.getRoot() + "/" + config_.getTarget());

  // buildDebugger(method);

  if (error_code_ > 200)
    status_code_ = error_code_;
  else if (!config_.isMethodAccepted(method))
  {
    status_code_ = 405;
    headers_["Allow"] = buildMethodList();
  }
  else if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize())
  {
    status_code_ = 413;
  }
  else if (config_.getAuth() != "off" && !checkAuth())
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
  std::cout << "PATH: " << file_->getFilePath() << std::endl;

  if (method == "GET" || method == "HEAD")
  {
    if (file_->is_directory())
    {
      std::cout << "Handling directory request\n";
      int ret = handleDirectoryRequest();
      if (ret == 200 || ret == 404)
        return ret;
    }
    if (!file_->is_directory())
    {
      std::cout << "Handling file request\n";
      int ret = handleFileRequest();
      if (ret == 403 || ret == 404)
        return ret;
    }
  }
  std::cout << "isCGI: " << isCgi(file_->getMimeExt()) << "\n";
  /// @note added this to handle cgi
  // Hi Alex, here is what to expect
  // if you look into the config file you will see how cgi is configured
  // cgi [ext] [filename]
  // so this isCgi() function will check if the file extension is in the cgi map
  // and return true or false accordingly
  // for example if you have a file called test.py is called this will check what its mapped against in the cgi map
  if (isCgi(file_->getMimeExt())) {
    
    return status_code_;
  }
  
  if (method == "PUT" || method == "POST")
  {
    handlePutPostRequest();
  }

  return (this->*(HttpResponse::methods_[method]))();
}

int HttpResponse::handleDirectoryRequest()
{
  std::vector<std::string> indexes = config_.getIndexes();
  std::string index = file_->find_index(indexes);
  std::string newPath;

  std::cout << "INDEX: " << index << std::endl;
  std::cout << "AUTOINDEX: " << config_.getAutoIndex() << std::endl;
  std::cout << "Index size: " << indexes.size() << std::endl;
  if (indexes.size() > 0)
  {
    for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
    {
      std::cout << "INDEX: " << *it << std::endl;
    }
  }
  std::cout << "File path: " << file_->getFilePath() << std::endl;
  std::cout << "File exists: " << file_->exists(file_->getFilePath() + "/" + indexes[0]) << std::endl;

  if (!index.empty())
  {
    redirect_ = true;
    newPath = "/" + config_.getRequestTarget();
    newPath += "/" + index;
    redirect_target_ = removeDupSlashes(newPath);

    return 200;
  }

  if (indexes.empty() && !config_.getAutoIndex())
  {
    std::cout << "DEBUG 1\n";
    return 404;
  }

  if (!config_.getAutoIndex() && !file_->exists(file_->getFilePath()))
  {
    std::cout << "DEBUG 2\n";
    return 404;
  }
  else if (!config_.getAutoIndex())
  {
    std::cout << "DEBUG 4\n";
    /// @note added this to handle autoindex
    if (file_->exists(file_->getFilePath()))
      return (config_.setAutoIndex(true), 0);
    return 404;
  }
  return (config_.setAutoIndex(true), 0);
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
    fullResponse += "\r\n\r\n";
    fullResponse += body_;
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

bool HttpResponse::isCgi(std::string ext) {
  std::map<std::string, std::string> &cgi = config_.getCgi();

  return cgi.find(ext) != cgi.end();
}

