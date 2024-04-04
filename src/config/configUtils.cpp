/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsFunc.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oduwoledare <oduwoledare@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 13:06:39 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/22 11:53:19 by oduwoledare      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

std::vector<std::string> customSplit(const std::string &s, char delim) {
    std::vector<std::string> port_list;
    std::stringstream iss(s);
    std::string port;
    while (std::getline(iss, port, delim)) {
        if(!port.empty() && port[0] != delim)
            port_list.push_back(port);
    }
    return port_list;
}

std::string getIndexVariableKey(std::string key,std::map<std::string, std::vector<std::string> > keyValues)
{
    typedef std::map<std::string, std::vector<std::string> >::const_iterator MapIterator;
    std::stringstream finalKey;
    MapIterator it;
    int index = 0;

	for(it = keyValues.begin(); it != keyValues.end(); it++)
    {
        if(it->first.substr(0, it->first.size() - 3) == key)
            ++index;
    }
    finalKey << "[" << index << "]";
    return finalKey.str();
}

// Function to trim whitespaces from end
void trimWordFromEnd(int &start, int &end, std::string line) {
    (void)start;
    end = line.size() - 1;
    while(isspace(line[end]) || line[end] == '{')
        end--;
}


// Checks closed curly braces or not
int checkCurly(std::string line) {
    int i = 0;
    int openCurly = 0;
    int closedCurly = 0;

    while(line[i]) {
        if (line[i] == '{')
            ++openCurly;
        else if (line[i] == '}')
            ++closedCurly;
        ++i;
    }
    if (openCurly == closedCurly)
        return 0;
    return 1;
}

void ft_errors(std::string arg, int i)
{
    if(i == 1)
        std::cerr << "Usage: " << arg << " <config_file>" << std::endl;
    if(i == 2)
        std::cerr << "Error opening file: " << arg << std::endl;
    if(i == 3)
        std::cerr << "Quotes are not closed." << std::endl;
    exit(0);
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string cutTillSemicolon(std::string str)
{
    size_t i = str.find_first_of(';');
    if(i != std::string::npos)
        str = str.substr(0, i);
    return str;
}

void    handleLogFormat(std::string line, std::string &value, std::vector<std::string> tokens, std::vector<std::string>::const_iterator &it)
{
    bool firstLine = true;
    std::string cutSemicolon;
    value = "";
    while (true)
    {
        line = *it;
        tokens = split(line, ' ');
        std::vector<std::string>::iterator token_it = tokens.begin();
        while (token_it != tokens.end())
        {
            if (firstLine)
            {
                value += *(++token_it) + " ";
                firstLine = false;
            }
            else
            {
                cutSemicolon = cutTillSemicolon(*(token_it++));
                value += cutSemicolon + " ";
            }
        }
        if (line[line.size() - 1] == ';' || line[line.size() - 1] == '}')
            break;
        it++;
    }
}


void printAllDBData(GroupedDBMap db) {
    GroupedDBMap::const_iterator it;
    for (it = db.begin(); it != db.end(); ++it) {
        std::cout << "Index: " << it->first << std::endl;
        printData(it->second);
        std::cout << "\n" << std::endl;
    }
}

void printData(const std::vector<ConfigDB::KeyMapValue>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        const MapStr& keyMap = values[i].first;
        const VecStr& valueVector = values[i].second;

            std::cout << "{ " 
                << keyMap.find("directives")->second
                << ", "  << keyMap.find("location")->second
                << " }" << ": ";

        for (size_t j = 0; j < valueVector.size(); ++j)
            std::cout << "  " << valueVector[j];

        std::cout  << std::endl;
    }
}

std::vector<KeyMapValue> getDataByIdx(GroupedDBMap db, int index) {
    std::vector<KeyMapValue> values;
    GroupedDBMap::iterator it = db.find(index);

    return (it != db.end())
        ? it->second
        : (std::cout << "Index " << index << " does not exist" << std::endl, values);
}

