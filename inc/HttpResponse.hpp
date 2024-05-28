/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:14:47 by alappas           #+#    #+#             */
/*   Updated: 2024/05/28 23:14:48 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "./AllHeaders.hpp"
#include "./File.hpp"

class HttpRequest;
class RequestConfig;
class File;

extern pthread_mutex_t g_write;

struct CharsetAndQ {
	std::string charset;
	double qValue;
};

class HttpResponse {
public:
	HttpResponse(RequestConfig &config, int error_code = 100);
	~HttpResponse();
	HttpResponse(const HttpResponse &rhs);
	HttpResponse(const HttpResponse &rhs, RequestConfig &config);
	HttpResponse &operator=(const HttpResponse &rhs);

	typedef int (HttpResponse::*type)();
	void initMethods();

	enum LogLevel {
		NONE,
		INFO,
		DEBUG
	};


	void cleanUp();
	int buildErrorPage(int status_code);
	void redirectToErrorPage(const std::string& target, int status_code);
	std::string buildDefaultErrorPage(int status_code);
	void build();
	int handleMethods();
	int handleDirectoryRequest();
	int handleFileRequest();
	void handlePutPostRequest();
	void handleAcceptLanguage(std::vector<std::string> &matches);
	void handleAcceptCharset(std::vector<std::string> &matches);
	int handleOtherMethods();
	void createResponse();
	bool localization(std::vector<std::string> &matches);
	std::string accept_charset(std::vector<std::string> &matches);
	bool getRedirect();
	std::string redirect_target();
	void printMethodMap();
	void setErrorPageHeaders(int status_code);
	int checkCustomErrorPage(int status_code);

	std::pair<std::string, double> extractLangAndQ(const std::string& langAndQ);
	std::string findBestLanguage(const std::vector<std::string>& matches, const std::vector<std::pair<std::string, double> >& langQPairs);

	CharsetAndQ extractCharsetAndQ(const std::string& charsetAndQ);
	std::string handleDefaultCharset(const std::string& bestCharset);
	std::string findBestCharset(const std::vector<CharsetAndQ>& charsetAndQValues, const std::vector<std::string>& matches);
	void buildDebugger (std::string method);

	std::string response_log(LogLevel level);

	int GET();
	int POST();
	int PUT();
	int DELETE();

	int getStatus();
	std::string getResponseBody();
	int sendResponse(int fd);
	std::string getSampleResponse();
	bool isCgi(std::string extension);

	void HandleCgi();
	void setCgiPipe(CgiHandle &cgi);
	void toCgi(CgiHandle &cgi, std::string &req_body);
	void fromCgi(CgiHandle &cgi);
	void handleCgiHeaders(std::string &body);
	void parseCgiHeaders();
	void closeParentCgiPipe(CgiHandle &cgi);
	void setStatusCode(int code);
	void setHeader(std::string key, std::string value);
	void appendBody(char *buffer, int size);
	std::string& getBody();
	void setBody(std::string body);
	File *getFile();
	std::map<std::string, std::string> getHeaders();
	void setSubstr(int start, std::size_t end);
	void setSubstr(int start);
	void clearBody();
	void setConfig(RequestConfig &config);
	std::map<std::string, std::string> &getHeadersRef();
	bool getCgiStatus();
	bool foundIndex;

private:
	RequestConfig &config_;
	File *file_;
	int error_code_;
	int worker_id_;
	size_t total_sent_;
	int status_code_;
	std::string response_;
	std::string body_;
	bool redirect_;
	std::string redirect_target_;
	int redirect_code_;
	size_t header_size_;
	size_t body_size_;
	std::string charset_;
	std::map<std::string, HttpResponse::type> methods_;
	std::pair<std::string, int> findLocation(std::string target);
	std::map<std::string, std::string> headers_;

	std::string cgiHeaders_;
	bool	cgiHeadersParsed_;
	bool	cgiRead_;
	int		cgi_bytes_read_;
	int		cgi_times_read_;
	bool	cgi_true_;
	std::string buildMethodList();
	bool checkAuth();
};

std::string getMimeType(const std::string& ext);
std::string getHttpStatusCode(int code);


#endif