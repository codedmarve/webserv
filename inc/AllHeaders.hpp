/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllHeaders.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 14:47:30 by nikitos           #+#    #+#             */
/*   Updated: 2024/05/12 17:54:01 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLHEADERS_HPP
#define ALLHEADERS_HPP

#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <set>
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <stack>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <regex.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>


#include "ConfigDB.hpp"
#include "Servers.hpp"
#include "CgiHandle.hpp"
#include "DomainResolve.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "File.hpp"
#include "MimeTypes.hpp"
#include "HttpStatusCode.hpp"
#include "RequestConfig.hpp"
#include "utils.hpp"
#include "Client.hpp"
#include "ErrorCodes.hpp"


typedef std::map<std::string, std::string> MapStr;
typedef std::vector<std::string> VecStr;
typedef std::map<std::string, VecStr> KeyValues;
typedef std::pair<MapStr, VecStr> KeyMapValue;
typedef std::map<int, std::vector<KeyMapValue> > GroupedDBMap;

struct DB
{
  const GroupedDBMap serversDB;
  const GroupedDBMap rootDB;
};

struct Listen
{
  std::string ip_;
  uint32_t port_;

  Listen() : ip_("127.0.0.1"), port_(80){};
  Listen(std::string ip, uint32_t port) : ip_(ip), port_(port){};
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
std::string getIndexVariableKey(std::string key, std::map<std::string, std::vector<std::string> > keyValues);
std::string getValue(const std::map<std::string, std::vector<std::string> > &keyValues, const std::string &key);

void printAllDBData(GroupedDBMap db);
void printData(const std::vector<KeyMapValue> &values);
std::vector<KeyMapValue> getDataByIdx(GroupedDBMap db, int index);
bool setModifier(std::string &str);
const std::string b64decode(const void *data, const size_t &len);
std::string b64decode(const std::string &str64);
std::string ftos(size_t num);
std::string removeDupSlashes(std::string str);
std::string formatHttpDate(time_t timeValue);
std::string get_http_date();
std::string md5(const std::string& input);
std::string generateETagForFile(File& file);
bool isMethodCharValid(char ch);
bool isAlpha(char c);
bool isDigit(char c);
bool isAlphaNum(char c);
bool isUnreserved(char c);
bool isSubDelim(char c);
bool isHexDigit(char c);
std::string trim(const std::string& str);
unsigned int hexToDecimal(const std::string& hex);

template<typename KeyType, typename ValueType>
void printMap(const std::map<KeyType, ValueType> &m);

template<typename T>
void printVec(const std::vector<T> &value, const std::string &callingFunction);

#endif
