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
  initMethodMap();
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

void HttpResponse::initMethodMap()
{
  // methods_["GET"] = &HttpResponse::GET;
  // methods_["HEAD"] = &Response::GET;
  // methods_["POST"] = &Response::POST;
  // methods_["PUT"] = &Response::PUT;
  // methods_["DELETE"] = &Response::DELETE;
}
// int HttpResponse::GET() {
//   if (config_.getAutoindex() && file_.is_directory()) {
//     // Autoindex response
//     headers_["Content-Type"] = "text/html; charset=UTF-8";
//     body_ = file_.autoIndex(config_.getRequestTarget());
//   }
//   else {
//     // Regular file response
//     headers_["Content-Type"] = g_mimes.getType(file_.getMimeExtension());
//     if (!charset_.empty()) {
//       headers_["Content-Type"] += "; charset=" + charset_;
//     }
//     body_ = file_.getContent();
//   }

//   // Set Content-Length based on body length
//   headers_["Content-Length"] = ft::to_string(body_.length());

//   return 200; // OK
// }


void HttpResponse::build()
{
  std::string &method = config_.getMethod();
  (void)method;
  file_ = new File();

  file_->set_path(config_.getRoot() + "/" + config_.getTarget());

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
  {
    return 404;
  }

  file_->findMatchingFiles();
  std::vector<std::string> &matches = file_->getMatches();

  handleAcceptLanguage(matches);
  handleAcceptCharset(matches);

  if (!file_->openFile())
  {
    return 403;
  }

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
  if (!config_.getHeader("Accept-Language").empty())
  {
    if (localization(matches))
    {
      file_->set_path(file_->getFilePath().substr(0, file_->getFilePath().find_last_of("/") + 1) + matches.front(), true);
    }
  }
}

void HttpResponse::handleAcceptCharset(std::vector<std::string> &matches)
{
  if (!config_.getHeader("Accept-Charset").empty())
  {
    charset_ = accept_charset(matches);
    file_->set_path(file_->getFilePath().substr(0, file_->getFilePath().find_last_of("/") + 1) + matches.front(), true);
  }
}

int HttpResponse::handleOtherMethods()
{
  std::cerr << "Method not implemented" << std::endl;
  return 405; 
}

int HttpResponse::buildErrorPage(int status_code) {
  // Check if a custom error page is configured for this status code
  const std::string& customErrorPage = config_.getErrorPages()[status_code];
  if (!customErrorPage.empty()) {
    std::string target = removeDupSlashes(customErrorPage);
    std::string cur_target = removeDupSlashes("/" + config_.getTarget());

    // If the custom error page is different from the current target, redirect
    if (target != cur_target) {
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

void HttpResponse::redirectToErrorPage(const std::string& target, int status_code) {
  config_.getMethod() = "GET";

  redirect_ = true;
  redirect_code_ = status_code;
  redirect_target_ = target;
}

std::string HttpResponse::buildDefaultErrorPage(int status_code) {
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

bool HttpResponse::getRedirect()
{
  return redirect_;
}

std::string HttpResponse::redirect_target()
{
  return redirect_target_;
}

/**
 * LOCALIZATION
 */

std::pair<std::string, double> HttpResponse::extractLangAndQ(const std::string &langAndQ)
{
  std::string lang;
  double qValue = 1.0;

  std::size_t qPos = langAndQ.find(";q=");
  if (qPos != std::string::npos)
  {
    lang = langAndQ.substr(0, qPos);
    std::string qStr = langAndQ.substr(qPos + 3);
    try
    {
      qValue = atof(qStr.c_str());
    }
    catch (...)
    {
      qValue = 0.0;
    }
  }
  else
  {
    lang = langAndQ;
  }

  return std::make_pair(lang, qValue);
}

std::string HttpResponse::findBestLanguage(const std::vector<std::string> &matches, const std::vector<std::pair<std::string, double> > &langQPairs)
{
  std::string bestLanguage;
  /// @note rates lang preference
  // e.g Accept-Language: en-US,en;q=0.8,fr;q=0.7
  // en-US: No specified q value, defaults to 1.0 (highest).
  // en;q=0.8: English with a q value of 0.8.
  // fr;q=0.7: French with a q value of 0.7.
  double bestQValue = 0.0;

  for (std::vector<std::pair<std::string, double> >::const_iterator it = langQPairs.begin(); it != langQPairs.end(); ++it)
  {
    const std::string &lang = it->first;
    double qValue = it->second;

    if (lang == "*")
    {
      bestLanguage = matches.front();
      bestQValue = qValue;
      break; // Use the first language as default
    }
    else
    {
      for (std::vector<std::string>::const_iterator matchIt = matches.begin(); matchIt != matches.end(); ++matchIt)
      {
        if (matchIt->find("." + lang) != std::string::npos && qValue > bestQValue)
        {
          bestLanguage = *matchIt;
          bestQValue = qValue;
          break; // Found a better match, no need to continue
        }
      }
    }
  }

  return bestLanguage;
}

bool HttpResponse::localization(std::vector<std::string> &matches)
{
  std::string all = config_.getHeader("Accept-Language");

  std::vector<std::pair<std::string, double> > langQPairs;
  std::size_t pos = 0;
  while (pos < all.length())
  {
    std::size_t nextComma = all.find(',', pos);
    std::string langAndQ = all.substr(pos, (nextComma == std::string::npos) ? std::string::npos : nextComma - pos);
    langQPairs.push_back(extractLangAndQ(langAndQ));

    if (nextComma == std::string::npos)
      break;
    else
      pos = nextComma + 1;
  }

  std::string bestLanguage = findBestLanguage(matches, langQPairs);

  if (bestLanguage.empty())
    bestLanguage = "en"; // Default to English

  if (!bestLanguage.empty())
  {
    matches.clear();
    matches.push_back(bestLanguage);
    headers_["Content-Language"] = bestLanguage;
    return true;
  }

  return false;
}

CharsetAndQ HttpResponse::extractCharsetAndQ(const std::string &charsetAndQ)
{
  CharsetAndQ entry;
  entry.qValue = 1.0;

  size_t qPos = charsetAndQ.find(";q=");
  if (qPos != std::string::npos)
  {
    entry.charset = charsetAndQ.substr(0, qPos);
    std::string qStr = charsetAndQ.substr(qPos + 3);
    try
    {
      entry.qValue = atof(qStr.c_str());
    }
    catch (...)
    {
      entry.qValue = 0.0;
    }
  }
  else
  {
    entry.charset = charsetAndQ;
  }

  return entry;
}

std::string HttpResponse::findBestCharset(const std::vector<CharsetAndQ> &charsetAndQValues, const std::vector<std::string> &matches)
{
  std::string bestCharset;
  double bestQValue = 0.0;

  for (size_t i = 0; i < charsetAndQValues.size(); ++i)
  {
    CharsetAndQ entry = charsetAndQValues[i];
    if (entry.charset == "*")
    {
      bestCharset = matches.front();
      bestQValue = entry.qValue;
      break;
    }
    else
    {
      for (size_t j = 0; j < matches.size(); ++j)
      {
        if (matches[j].find("." + entry.charset) != std::string::npos && entry.qValue > bestQValue)
        {
          bestCharset = matches[j];
          bestQValue = entry.qValue;
          break;
        }
      }
    }
  }

  return bestCharset;
}

std::string HttpResponse::handleDefaultCharset(const std::string &bestCharset)
{
  return (bestCharset.empty()) ? "utf-8" : bestCharset;
}

std::string HttpResponse::accept_charset(std::vector<std::string> &matches)
{
  std::string all = config_.getHeader("Accept-Charset");
  std::vector<CharsetAndQ> charsetAndQValues;

  size_t pos = 0;
  while (pos < all.length())
  {
    size_t nextComma = all.find(',', pos);
    size_t end = (nextComma == std::string::npos) ? all.length() : nextComma;
    std::string charsetAndQ = all.substr(pos, end - pos);

    CharsetAndQ entry = extractCharsetAndQ(charsetAndQ);
    charsetAndQValues.push_back(entry);

    if (nextComma == std::string::npos)
    {
      break;
    }
    else
    {
      pos = nextComma + 1;
    }
  }

  std::string bestCharset = findBestCharset(charsetAndQValues, matches);

  return handleDefaultCharset(bestCharset);
}

void HttpResponse::createResponse() {
    headers_["Server"] = "webserv/1.0";

    if (config_.getMethod() == "HEAD") {
        body_.clear();
    }

    if (status_code_ < 400 && redirect_code_) {
        status_code_ = redirect_code_;
    }

    std::string status_code_phrase = file_->getStatusCode(status_code_);
    std::string status_line = "HTTP/1.1 " + ftos(status_code_) + " " + status_code_phrase + "\r\n";
    
    headers_["Date"] = get_http_date();

    std::string header_block;
    for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++) {
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

