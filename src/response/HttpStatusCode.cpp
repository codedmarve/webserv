/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatusCode.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:16:11 by alappas           #+#    #+#             */
/*   Updated: 2024/05/28 23:16:12 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

HttpStatusCodes::HttpStatusCodes() {
    // 1xx Informational
    codeMap[100] = "Continue";
    codeMap[101] = "Switching Protocols";
    codeMap[102] = "Processing";

    // 2xx Success
    codeMap[200] = "OK";
    codeMap[201] = "Created";
    codeMap[202] = "Accepted";
    codeMap[203] = "Non-Authoritative Information";
    codeMap[204] = "No Content";
    codeMap[205] = "Reset Content";
    codeMap[206] = "Partial Content";
    codeMap[207] = "Multi-Status";
    codeMap[208] = "Already Reported";
    codeMap[226] = "IM Used";

    // 3xx Redirection
    codeMap[300] = "Multiple Choices";
    codeMap[301] = "Moved Permanently";
    codeMap[302] = "Found";
    codeMap[303] = "See Other";
    codeMap[304] = "Not Modified";
    codeMap[305] = "Use Proxy";
    codeMap[307] = "Temporary Redirect";
    codeMap[308] = "Permanent Redirect";

    // 4xx Client Error
    codeMap[400] = "Bad Request";
    codeMap[401] = "Unauthorized";
    codeMap[402] = "Payment Required";
    codeMap[403] = "Forbidden";
    codeMap[404] = "Not Found";
    codeMap[405] = "Method Not Allowed";
    codeMap[406] = "Not Acceptable";
    codeMap[407] = "Proxy Authentication Required";
    codeMap[408] = "Request Timeout";
    codeMap[409] = "Conflict";
    codeMap[410] = "Gone";
    codeMap[411] = "Length Required";
    codeMap[412] = "Precondition Failed";
    codeMap[413] = "Payload Too Large";
    codeMap[414] = "URI Too Long";
    codeMap[415] = "Unsupported Media Type";
    codeMap[416] = "Range Not Satisfiable";
    codeMap[417] = "Expectation Failed";
    codeMap[418] = "I'm a teapot";
    codeMap[421] = "Misdirected Request";
    codeMap[422] = "Unprocessable Entity";
    codeMap[423] = "Locked";
    codeMap[424] = "Failed Dependency";
    codeMap[426] = "Upgrade Required";
    codeMap[428] = "Precondition Required";
    codeMap[429] = "Too Many Requests";
    codeMap[431] = "Request Header Fields Too Large";
    codeMap[451] = "Unavailable For Legal Reasons";

    // 5xx Server Error
    codeMap[500] = "Internal Server Error";
    codeMap[501] = "Not Implemented";
    codeMap[502] = "Bad Gateway";
    codeMap[503] = "Service Unavailable";
    codeMap[504] = "Gateway Timeout";
    codeMap[505] = "HTTP Version Not Supported";
    codeMap[506] = "Variant Also Negotiates";
    codeMap[507] = "Insufficient Storage";
    codeMap[508] = "Loop Detected";
    codeMap[510] = "Not Extended";
    codeMap[511] = "Network Authentication Required";
}

HttpStatusCodes::HttpStatusCodes(const HttpStatusCodes &rhs)
    : codeMap(rhs.codeMap) {
}

HttpStatusCodes &HttpStatusCodes::operator=(const HttpStatusCodes &rhs) {
    if (this != &rhs) {
        codeMap = rhs.codeMap;
    }
    return *this;
}

std::string HttpStatusCodes::getStatusCode(int code) {
    std::map<int, std::string>::iterator it = codeMap.find(code);
    return (it != codeMap.end()) ? it->second : "Unknown";
}

HttpStatusCodes::~HttpStatusCodes() {
    codeMap.clear();
}