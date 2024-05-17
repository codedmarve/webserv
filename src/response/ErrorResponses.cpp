#include "../../inc/HttpResponse.hpp"

int HttpResponse::buildErrorPage(int status_code)
{
    if (checkCustomErrorPage(status_code) == 0)
        return 0;

    body_ = buildDefaultErrorPage(status_code);
    setErrorPageHeaders(status_code);

    return status_code;
}

void HttpResponse::setErrorPageHeaders(int status_code)
{
    if (status_code < 400 || status_code >= 600)
        throw std::invalid_argument("Invalid HTTP status code");

    switch (status_code)
    {
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
    headers_["Content-Type"] = file_->getMimeType(".html");
    headers_["Content-Length"] = ftos(body_.length());
}

void HttpResponse::redirectToErrorPage(const std::string &target, int status_code)
{
    config_.getMethod() = "GET";

    redirect_ = true;
    redirect_code_ = status_code;
    redirect_target_ = target;
}

int HttpResponse::checkCustomErrorPage(int status_code)
{

    const std::string &customErrorPage = config_.getErrorPages()[status_code];
    if (!customErrorPage.empty())
    {
        std::string target = removeDupSlashes(customErrorPage);
        std::string cur_target = removeDupSlashes("/" + config_.getTarget());

        if (target != cur_target)
            return (redirectToErrorPage(target, status_code), 0);
    }
    return 1;
}

std::string HttpResponse::buildDefaultErrorPage(int status_code)
{
    std::string errorPage;

    errorPage.append("<html>\r\n");
    errorPage.append("<head>\r\n");
    errorPage.append("<title>" + ftos(status_code) + " " + file_->getStatusCode(status_code) + "</title>\r\n");
    errorPage.append("<meta charset=\"utf-8\">\r\n");
    errorPage.append("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n");
    errorPage.append("<meta name=\"description\" content=\"" + file_->getStatusCode(status_code) + "\">\r\n");
    errorPage.append("<meta name=\"author\" content=\"Your Website\">\r\n");
    errorPage.append("</head>\r\n");
    errorPage.append("<body>\r\n");
    errorPage.append("<center><h1>" + ftos(status_code) + " " + file_->getStatusCode(status_code) + "</h1></center>\r\n");
    errorPage.append("<hr><center>" + headers_["Server"] + "</center>\r\n");
    errorPage.append("</body>\r\n");
    errorPage.append("</html>\r\n");

    return errorPage;
}