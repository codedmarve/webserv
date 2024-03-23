

int Response::buildErrorPage(int status_code) {
    // Check if custom error page is defined for the status code
    const std::string& errorPage = config_.getErrorPages()[status_code];
    if (!errorPage.empty()) {
        std::string target = "/" + errorPage; // Assuming errorPage is already a valid path
        std::string cur_target = "/" + config_.getTarget();

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
    bodyStream << "<head><title>" << status_code << " " << g_status[status_code] << "</title></head>\r\n";
    bodyStream << "<body>\r\n";
    bodyStream << "<center><h1>" << status_code << " " << g_status[status_code] << "</h1></center>\r\n";
    
    // Check if Server header is available
    if (headers_.count("Server") > 0) {
        bodyStream << "<hr><center>" << headers_["Server"] << "</center>\r\n";
    }
    
    bodyStream << "</body>\r\n";
    bodyStream << "</html>\r\n";

    std::string body = bodyStream.str();

    // Set headers for the error page
    headers_["Content-Type"] = g_mimes.getType(".html");
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
