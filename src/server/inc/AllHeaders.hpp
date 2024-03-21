/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AllHeaders.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: novsiann <novsiann@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 14:47:30 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/17 13:33:09 by novsiann         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ALLHEADERS_HPP
# define ALLHEADERS_HPP

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <limits.h>
#include <iomanip>
#include "DataBase.hpp"
#include "Servers.hpp"
#include "CgiHandle.hpp"
#include "DomainResolve.hpp"

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

#endif
