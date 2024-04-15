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

bool HttpResponse::shouldDisconnect() {
    return headers_.find("Connection") != headers_.end() && headers_["Connection"] == "close";
}

void HttpResponse::printMethodMap() {
    std::map<std::string, int (HttpResponse::*)()>::iterator it;

    for (it = methods_.begin(); it != methods_.end(); ++it) {
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
    createResponse();
}

int HttpResponse::handleMethods()
{
  std::string &method = config_.getMethod();

  if (method == "GET" || method == "HEAD")
  {
    return (file_->is_directory()) ? handleDirectoryRequest() : handleFileRequest();
  }
  else if (method == "PUT" || method == "POST")
  {
    return handlePutPostRequest();
  }
  return handleOtherMethods();
}

int HttpResponse::handleDirectoryRequest()
{
  std::string index = file_->find_index(config_.getIndexes());
  if (!index.empty())
  {
    redirect_ = true;
    redirect_target_ = removeDupSlashes("/" + config_.getRequestTarget() + "/" + index);
    return 301;
  }
  else if (!config_.getAutoIndex())
  {
    return 404;
  }

  return 200;
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

  return 200;
}

int HttpResponse::handlePutPostRequest()
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
        return 500;
      }
    }
    file_->set_path(dir.getFilePath() + "/" + config_.getTarget());
  }
  headers_["Location"] = removeDupSlashes(path);

  return 201; // Created
}

void HttpResponse::handleAcceptLanguage(std::vector<std::string> &matches)
{
  std::string path = file_->getFilePath();

  if (!config_.getHeader("Accept-Language").empty())
  {
    if (localization(matches))
    {
      file_->set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front(), true);
    }
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

int HttpResponse::buildErrorPage(int status_code)
{
  // Check if a custom error page is configured for this status code
  const std::string &customErrorPage = config_.getErrorPages()[status_code];
  if (!customErrorPage.empty())
  {
    std::string target = removeDupSlashes(customErrorPage);
    std::string cur_target = removeDupSlashes("/" + config_.getTarget());

    // If the custom error page is different from the current target, redirect
    if (target != cur_target)
    {
      redirectToErrorPage(target, status_code);
      return 0;
    }
  }

  body_ = buildDefaultErrorPage(status_code);

  // Set appropriate headers
  headers_["Content-Type"] = file_->getMimeType(".html");
  headers_["Content-Length"] = ftos(body_.length());

  // Additional headers for specific status codes
  if (status_code == 401)
    headers_["WWW-Authenticate"] = "Basic realm=\"Access to restricted area\"";
  if (status_code == 408 || status_code == 503)
    headers_["Connection"] = "close";
  if (status_code == 503)
    headers_["Retry-After"] = "30";

  return status_code;
}

void HttpResponse::redirectToErrorPage(const std::string &target, int status_code)
{
  config_.getMethod() = "GET";

  redirect_ = true;
  redirect_code_ = status_code;
  redirect_target_ = target;
}

std::string HttpResponse::buildDefaultErrorPage(int status_code)
{
  std::string errorPage;
  errorPage += "<html>\r\n";
  errorPage += "<head>\r\n";
  errorPage += "<title>" + ftos(status_code) + " " + file_->getStatusCode(status_code) + "</title>\r\n";
  errorPage += "<meta charset=\"utf-8\">\r\n";
  errorPage += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n";
  errorPage += "<meta name=\"description\" content=\"" + file_->getStatusCode(status_code) + "\">\r\n";
  errorPage += "<meta name=\"author\" content=\"Your Website\">\r\n";
  errorPage += "</head>\r\n";
  errorPage += "<body>\r\n";
  errorPage += "<center><h1>" + ftos(status_code) + " " + file_->getStatusCode(status_code) + "</h1></center>\r\n";
  errorPage += "<hr><center>" + headers_["Server"] + "</center>\r\n";
  errorPage += "</body>\r\n";
  errorPage += "</html>\r\n";
  return errorPage;
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

std::string HttpResponse::response_log(LogLevel level) {
    std::string ret;

    if (level == INFO) {
        ret = "[status: " + ftos(status_code_) + " " + file_->getStatusCode(status_code_) + "]";
        if (headers_.count("Content-Length"))
            ret = ret + " [length: " + headers_["Content-Length"] + "]";
    } else if (level > INFO) {
        ret = "\n\n" + response_.substr(0, header_size_ - 4) + "\n";
        if (body_size_) {
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
  headers_["Server"] = "webserv/1.0";

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
}

int HttpResponse::sendResponse(int fd) {
    std::string fullResponse = response_;
    if (!body_.empty()) {
        headers_["Content-Length"] = ftos(body_.length());
        fullResponse += "\r\n\r\n";
        fullResponse += body_;
    }

    int ret = send(fd, fullResponse.c_str() + total_sent_, fullResponse.length() - total_sent_, 0);
    if (ret <= 0) {
        std::cerr << "send : " << strerror(errno) << std::endl;
        return -1;
    }

    total_sent_ += ret;
    if (total_sent_ >= fullResponse.length())
        return 0;

    return 1;
}
