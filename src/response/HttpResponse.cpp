#include "../../inc/AllHeaders.hpp"

Response::Response(RequestConfig &config, int worker_id, int error_code) : config_(config), worker_id_(worker_id) {
  error_code_ = error_code;
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_code_ = 0;
  redirect_ = false;
  charset_ = "";
  initMethodMap();
}

Response::~Response() {}

void Response::clear() {
  error_code_ = 0;
  status_code_ = 0;
  total_sent_ = 0;
  header_size_ = 0;
  body_size_ = 0;
  redirect_ = false;
  response_.clear();
  body_.clear();
  headers_.clear();
}



int Response::buildErrorPage(int status_code) {
    // Check if custom error page is defined for the status code
    const std::string& errorPage = config_.getErrorPages()[status_code]; // get errorpage from 
    if (!errorPage.empty()) {
        std::string target = "/" + errorPage; // Assuming errorPage is already a valid path
        std::string cur_target = "/" + getPath();

        // Redirect to custom error page if different from current target
        if (target != cur_target) {
            config_.setMethod("GET");
            redirect_ = true;
            redirect_code_ = status_code;
            redirect_target_ = target;
            return 0;
        }
    }

    // Default error page
    std::ostringstream bodyStream;
    bodyStream << "<html>\r\n";
    bodyStream << "<head><title>" << status_code << " " << getHttpStatusCode(status_code) << "</title></head>\r\n";
    bodyStream << "<body>\r\n";
    bodyStream << "<center><h1>" << status_code << " " << getHttpStatusCode(status_code) << "</h1></center>\r\n";
    
    // Check if Server header is available
    if (headers_.count("Server") > 0) {
        bodyStream << "<hr><center>" << headers_["Server"] << "</center>\r\n";
    }
    
    bodyStream << "</body>\r\n";
    bodyStream << "</html>\r\n";

    std::string body = bodyStream.str();

    // Set headers for the error page
    headers_["Content-Type"] = getMimeType(".html");
    headers_["Content-Length"] = ft::to_string(body.length());

    // Additional headers based on status code
    switch (status_code) {
        case 401:
            headers_["WWW-Authenticate"] = "Basic realm=\"Access to restricted area\"";
            break;
        case 408:
        case 503:
            headers_["Connection"] = "close";
            if (status_code == 503)
                headers_["Retry-After"] = "30";
            break;
        default:
            break;
    }

    return status_code;
}

void logError(const std::string& message) {
  std::cerr << "ERROR: " << message << std::endl;
  // Here you can log to a file or a monitoring service
}


int GET() {
    // Check if file exists
    if (!file_.exists()) {
      logError("File Not Found: " + file_.getPath());
      return 404; // File Not Found
    }

    std::string status_line = "HTTP/1.1 200 OK\r\n";

    // Set Date header
    headers_["Date"] = getCurrentDateTime();

    // Check if autoindex is enabled and file is a directory
    if (config_.getAutoindex() && file_.is_directory()) {
      headers_["Content-Type"] = "text/html"; // MIME type for HTML
      body_ = file_.autoIndex(config_.getRequestTarget());
    } else {
      // Determine MIME type based on file extension
      std::string mime_type = getMimeType(file_.getMimeExtension());
      if (mime_type.empty()) {
        mime_type = "application/octet-stream"; // Default MIME type
      }
      headers_["Content-Type"] = mime_type;

      body_ = file_.getContent();
    }

    // Set Content-Length header
    headers_["Content-Length"] = std::to_string(body_.length());

    // Content Compression
    if (shouldCompress()) {
      headers_["Content-Encoding"] = "gzip";
      body_ = compressBody(body_);
    }

    // Security Headers
    headers_["X-Content-Type-Options"] = "nosniff";
    headers_["X-Frame-Options"] = "DENY";
    headers_["Content-Security-Policy"] = "default-src 'self'";

    // Construct the full response
    std::ostringstream response_stream;
    response_stream << status_line;
    for (std::map<std::string, std::string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
      response_stream << it->first << ": " << it->second << "\r\n";
    }
    response_stream << "\r\n"; // End of headers
    response_stream << body_;

    // Update the response string
    response_ = response_stream.str();

    return 200;
  }

std::string Response::getCurrentDateTime() {
  char buffer[80];
  std::time_t now = std::time(nullptr);
  std::strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
  return buffer;
}

bool Response::shouldDisconnect() {
  if (headers_.find("Connection") != headers_.end() && headers_["Connection"] == "close")
    return true;
  return false;
}

bool Response::redirect() {
  return redirect_;
}

std::string Response::redirect_target() {
  return redirect_target_;
}

int Response::send(int fd) {
  size_t bytes_left = response_.length() - total_sent_;
  const char* data_ptr = response_.c_str() + total_sent_;

  while (bytes_left > 0) {
    int ret = ::send(fd, data_ptr, bytes_left, 0);

    if (ret <= 0) {
      std::cerr << "send error: " << strerror(errno) << std::endl;
      return -1;  // Error
    }

    total_sent_ += ret;
    bytes_left -= ret;
    data_ptr += ret;
  }

  // All data sent
  return 0;
}


std::string Response::response_log(LogLevel level) {
  std::string ret;

  if (level == INFO) {
    ret = "[status: " + ft::to_string(status_code_) + " " + getHttpStatusCode(status_code_) + "]";
    if (headers_.count("Content-Length")) {
      ret += " [length: " + headers_["Content-Length"] + "]";
    }
  } else if (level > INFO) {
    ret = "\n\n[status: " + ft::to_string(status_code_) + " " + getHttpStatusCode(status_code_) + "]\n";
    if (!response_.empty()) {
      size_t max_header_size = std::min(header_size_, response_.size());
      ret += response_.substr(0, max_header_size);

      if (body_size_ > 0 && response_.size() > header_size_) {
        size_t max_body_size = std::min(body_size_, response_.size() - header_size_);
        if (max_body_size < 200) {
          ret += "\n" + response_.substr(header_size_);
        } else {
          ret += "\n" + response_.substr(header_size_, 200) + "...";
        }
      }
    }
  }
  return ret;
}

void Response::createResponse() {
  // Set server header
  headers_["Server"] = "webserv/1.0";

  // Clear body for HEAD request
  if (config_.getMethod() == "HEAD") {
    body_.clear();
  }

  // Handle redirection status
  if (status_code_ < 400 && redirect_code_) {
    status_code_ = redirect_code_;
  }

  // Get status code
  std::string status_code_str;
  if (headers_.count("Status")) {
    status_code_str = headers_["Status"];
    headers_.erase("Status");
  } else {
    status_code_str = ft::to_string(status_code_) + " " + g_status[status_code_];
  }

  // Construct response line
  response_ = config_.getProtocol() + " " + status_code_str + "\r\n";

  // Set Date header
  headers_["Date"] = ft::get_http_date();

  // Append headers to response
  for (const auto& header : headers_) {
    response_ += header.first + ": " + header.second + "\r\n";
  }

  // Add end of headers marker
  response_ += "\r\n";

  // Calculate header size
  header_size_ = response_.size();

  // Append body if not empty
  if (!body_.empty()) {
    response_ += body_;
    body_size_ = body_.size();
    body_.clear();  // Clear body after appending
  }
}


int Response::handleMethods() {
  std::string& method = config_.getMethod();
  std::string path;

  if (method == "GET" || method == "HEAD") {
    if (file_.is_directory()) {
      std::string index = file_.find_index(config_.getIndexes());
      if (!index.empty()) {
        redirect_ = true;
        redirect_target_ = ft::unique_char("/" + config_.getRequestTarget() + "/" + index);
        return 200;
      } else if (!config_.getAutoindex()) {
        return 404;
      }
    }

    path = file_.getPath();
    if (!file_.is_directory()) {
      if (!file_.exists()) {
        return 404;
      }
      file_.parse_match();
      std::vector<std::string>& matches = file_.getMatches();

      // Handle Accept-Language and Accept-Charset
      handleContentNegotiation(matches, path);

      if (!file_.open()) {
        return 403;
      }

      headers_["Last-Modified"] = file_.last_modified();
    }
  }

  // Handle CGI execution
  if (isCGI(file_.getMimeExtension())) {
    return handleCGIExecution();
  }

  // Handle PUT and POST methods
  if (method == "PUT" || method == "POST") {
    return handlePutPost();
  }

  // Invoke appropriate method based on HTTP method (GET/POST/PUT etc.)
  return (this->*(Response::methods_[method]))();
}

void Response::handleContentNegotiation(const std::vector<std::string>& matches, const std::string& path) {
  if (!config_.getHeader("Accept-Language").empty()) {
    if (localization(matches)) {
      file_.set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front(), true);
    }
  }
  if (!config_.getHeader("Accept-Charset").empty()) {
    charset_ = accept_charset(matches);
    file_.set_path(path.substr(0, path.find_last_of("/") + 1) + matches.front(), true);
  }
}

int Response::handleCGIExecution() {
  CGI cgi(file_, config_, config_.getHeaders(), config_.getBody());
  cgi.init(worker_id_);
  int cgi_status = cgi.execute();
  if (cgi_status > 400) {
    return cgi_status;
  }
  cgi.parseHeaders(headers_);
  body_ = cgi.getBody();
  headers_["Content-Length"] = ft::to_string(body_.length());
  return cgi_status;
}

int Response::handlePutPost() {
  std::string path = config_.getUri() + "/" + config_.getTarget();

  if (!config_.getUpload().empty()) {
    File dir(config_.getRoot() + "/" + config_.getUpload());

    path = config_.getUri() + "/" + config_.getUpload() + "/" + config_.getTarget();
    if (dir.exists() && !dir.is_directory()) {
      dir.unlink();
    }

    if (!dir.exists()) {
      if (mkdir(dir.getPath().c_str(), 0755) == -1) {
        Log.print(DEBUG, "mkdir : " + std::string(strerror(errno)), RED, true);
      }
    }
    file_.set_path(dir.getPath() + "/" + config_.getTarget());
  }
  headers_["Location"] = ft::unique_char(path);
  return 303;  // Redirect status code
}

void Response::build() {
  std::string& method = config_.getMethod();

  // Set file path
  file_.set_path(config_.getRoot() + "/" + config_.getTarget());

  // Check for custom error code
  if (error_code_ > 1) {
    status_code_ = error_code_;
  } else {
    // Check if method is allowed
    if (!config_.methodAccepted(method)) {
      status_code_ = 405;
      headers_["Allow"] = buildMethodList();
    } else {
      // Check client body size limit
      if (config_.getClientMaxBodySize() > 0 && config_.getBody().length() > config_.getClientMaxBodySize()) {
        status_code_ = 413;
      } else {
        // Check authentication
        if (config_.getAuth() != "off" && !checkAuth()) {
          status_code_ = 401;
        } else {
          // Handle methods (GET, POST, PUT, etc.)
          status_code_ = handleMethods();
        }
      }
    }
  }

  // Build error page if needed
  if (status_code_ >= 300 && body_.empty()) {
    status_code_ = buildErrorPage(status_code_);
  }

  // Create response unless it's a redirect
  if (!redirect_) {
    createResponse();
  }
}

std::string Response::buildMethodList() {
  std::vector<std::string> methods = config_.getMethods();
  std::ostringstream oss;

  for (size_t i = 0; i < methods.size(); ++i) {
    oss << methods[i];
    if (i < methods.size() - 1) {
      oss << ", ";
    }
  }

  return oss.str();
}

bool Response::checkAuth() {
  std::string authorizationHeader = config_.getHeader("Authorization");

  if (authorizationHeader.empty()) {
    return false;  // No Authorization header provided
  }

  // Extract token from Authorization header
  size_t spacePos = authorizationHeader.find(' ');
  if (spacePos == std::string::npos) {
    return false;  // Invalid Authorization header format
  }

  std::string token = ft::b64decode(authorizationHeader.substr(spacePos + 1));

  // Check if token matches configured auth
  return (token == config_.getAuth());
}


bool Response::isCGI(std::string extension) {
  const std::map<std::string, std::string>& cgi = config_.getCGI();

  // Check if the extension exists in the CGI map
  std::map<std::string, std::string>::const_iterator it;
  for (it = cgi.begin(); it != cgi.end(); ++it) {
    if (it->first == extension) {
      return true;
    }
  }

  return false;
}

void Response::initMethodMap() {
  methods_["GET"] = &Response::GET;
  methods_["HEAD"] = &Response::GET;
  methods_["POST"] = &Response::POST;
  methods_["PUT"] = &Response::PUT;
  methods_["DELETE"] = &Response::DELETE;
}

