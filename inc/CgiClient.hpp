/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiClient.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 00:02:06 by alappas           #+#    #+#             */
/*   Updated: 2024/05/26 14:47:20 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_CLIENT_CPP
# define CGI_CLIENT_CPP

#include "./AllHeaders.hpp"

class Client;
class RequestConfig;
class CgiHandle;
class HttpResponse;

class CgiClient
{
private:
	Client			client_;
	std::string 	cgiHeaders_;
	std::map<std::string, std::string> *cgiHeadersMap_;
	bool			cgiHeadersParsed_;
	bool			cgiRead_;
	int				cgi_bytes_read_;
	int		    	cgi_times_read_;
	std::string 	cgi_ext_;
	RequestConfig	*config_;
	CgiHandle		*cgi_;
    HttpResponse    *response_;
	pid_t			pid_;
    int				status_code_;
    std::string     *req_body_;
    std::string     *body_;
	int				epoll_fd_;
public:
	CgiClient(Client client, int epoll_fd);
	~CgiClient();
	CgiClient(const CgiClient &other);
	CgiClient &operator=(const CgiClient &other);
	void HandleCgi();
	void setCgiPipe(CgiHandle &cgi);
	void toCgi(CgiHandle &cgi, std::string &req_body);
	void fromCgi(CgiHandle &cgi);
	void handleCgiHeaders(std::string &body);
	void parseCgiHeaders();
	void closeParentCgiPipe(CgiHandle &cgi);
    HttpResponse &getResponse();
    std::string getResponseString();
    int getStatusCode();
	void setContentLength(void);
	int getPipeOut(void);
	void deleteChild(int child_fd);
};

#endif