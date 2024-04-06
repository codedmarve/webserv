/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllHeaders.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 14:47:30 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/31 15:56:20 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLHEADERS_HPP
# define ALLHEADERS_HPP


#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <set>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
# include <regex.h>
#include <stack>
#include <string>

#include "ConfigDB.hpp"
#include "Servers.hpp"
#include "CgiHandle.hpp"
#include "DomainResolve.hpp"
#include "HttpRequestParser.hpp"
#include "HttpResponse.hpp"
#include "MimeTypes.hpp"
#include "HttpStatusCode.hpp"
#include "RequestConfig.hpp"
#include "utils.hpp"
#include "ServerConfig.hpp"
#include "Client.hpp"
#include "InputArgs.hpp"

typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr > KeyValues;
typedef std::pair<MapStr, VecStr > KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue > > GroupedDBMap;

struct DB {
    const GroupedDBMap serversDB;
    const GroupedDBMap rootDB;
};

struct Listen {
  std::string ip_;
  uint32_t port_;

  Listen() : ip_("127.0.0.1"), port_(80) {};
  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port) {};
};



// void
void trimWordFromEnd(int &start, int &end, std::string line);
void handleLogFormat(std::string line, std::string &value, std::vector<std::string> tokens, std::vector<std::string>::const_iterator &it);
void ft_errors(std::string arg, int i);

// int
int checkCurly(std::string line);

// vector
std::vector<std::string> customSplit(const std::string &s, char delim);
std::vector<std::string> split(const std::string &s, char delimiter);

// std::string
std::string cutTillSemicolon(std::string str);
std::string getIndexVariableKey(std::string key,std::map<std::string, std::vector<std::string> > keyValues);
std::string getValue(const std::map<std::string, std::vector<std::string> > &keyValues, const std::string &key);

void printAllDBData(GroupedDBMap db);
void printData(const std::vector<KeyMapValue>& values);
std::vector<KeyMapValue> getDataByIdx(GroupedDBMap db, int index);
bool checkModifier(std::string &str);

#endif
