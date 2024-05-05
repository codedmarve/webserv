/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileDB.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 11:19:53 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/31 18:37:48 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ConfigDB.hpp"

ConfigDB::ConfigDB()
{
    counter = 0;
}

ConfigDB::~ConfigDB() {}

void ConfigDB::pushInBase(std::string env_name)
{
    this->_variablePath.push_back(env_name);
}

void ConfigDB::eraseLastSection()
{
    if (!this->_variablePath.empty()) // Probably we don't need that condition
        this->_variablePath.pop_back();
}

std::string ConfigDB::getFullPathKey()
{
    VecStr::iterator it;
    std::string finalKey;

    for (it = _variablePath.begin(); it != _variablePath.end(); it++)
        finalKey += *it + ".";
    return finalKey;
}

std::string ConfigDB::getKeyWithoutLastSection()
{
    VecStr::iterator it;
    VecStr tmp = this->_variablePath;
    std::string finalKey;

    if (!tmp.empty())
    {
        tmp.pop_back();
    }
    for (it = tmp.begin(); it != tmp.end(); it++)
        finalKey += *it + ".";
    return finalKey;
}

std::string ConfigDB::readFile(char **argv)
{
    std::string configData;
    std::string lastLine;
    std::string line;
    std::string configFile = argv[1];
    std::ifstream file(configFile.c_str());

    if (!file)
        ft_errors(configFile, 2);

    while (std::getline(file, line))
    {
        std::size_t commentPos = line.find('#');
        if (commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos);
        }
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); // Trim leading whitespace
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // Trim trailing whitespace

        // If the line ends with a space, concatenate with the next line
        if (!line.empty() && line[line.size() - 1] == ' ')
        {
            lastLine += line.substr(0, line.size() - 1) + " ";
        }
        else
        {
            if (!lastLine.empty())
            {
                lastLine += line;
                configData += lastLine + "\n";
                lastLine.clear();
            }
            else
            {
                configData += line + "\n";
            }
        }
    }
    if (!lastLine.empty())
    {
        configData += lastLine;
    }
    if (checkCurly(configData))
        ft_errors("curly ", 3);
    return configData;
}

std::string ConfigDB::handleKeySection(int &start, int &end, std::string &line)
{
    std::string currentSection = "";

    trimWordFromEnd(start, end, line);
    currentSection = line.substr(start, end + 1);
    std::replace(currentSection.begin(), currentSection.end(), ' ', '_');
    this->pushInBase(currentSection);
    if (sectionCounts.find(currentSection) == sectionCounts.end())
    {
        sectionCounts[currentSection] = 0;
    }
    else
    {
        sectionCounts[currentSection]++;
    }
    std::vector<std::string>::iterator it = this->_variablePath.begin();
    bool server = false;
    while (it != this->_variablePath.end())
    {
        if (*it == "server")
            server = true;
        it++;
    }
    if (sectionCounts[currentSection] >= 0)
    {
        std::stringstream ss;
        ss << "[" << ((!server) ? sectionCounts[currentSection] : sectionCounts["server"]) << "]";
        currentSection += ss.str();
    }
    return currentSection;
}

void ConfigDB::printKeyValue()
{
    typedef std::map<std::string, VecStr>::const_iterator MapIterator;

    for (MapIterator it1 = _keyValues.begin(); it1 != _keyValues.end(); ++it1)
    {
        std::cout << "Key: " << it1->first
                  << "\nValue(s): " << std::endl;
        const VecStr &values = it1->second;
        for (VecStr::const_iterator it2 = values.begin(); it2 != values.end(); ++it2)
        {
            std::cout << "  " << *it2 << std::endl;
        }
        std::cout << "\n";
    }
}

void ConfigDB::fillMap(std::string value, std::string key, std::string currentSection, std::string KeyWithoutLastSection)
{
    VecStr splitedValue = split(value, ' ');
    VecStr::iterator spltValIt = splitedValue.begin();

    while (spltValIt != splitedValue.end())
    {
        if (!currentSection.empty())
        {
            _keyValues[KeyWithoutLastSection + currentSection + "." + key].push_back(*spltValIt);
        }
        else
        {
            std::string indexKey = getIndexVariableKey(key, _keyValues);
            while (spltValIt != splitedValue.end())
            {
                _keyValues[key + indexKey].push_back(*spltValIt);
                spltValIt++;
            }
            break;
        }
        spltValIt++;
    }
}
void ConfigDB::splitDirectiveAndValue(std::string currentSection, VecStr::const_iterator it, std::string trimmedLine)
{
    std::string value;
    std::string key;

    size_t spacePos = trimmedLine.find(" ");
    if (spacePos == std::string::npos) {
        key = cutTillSemicolon(trimmedLine);
        value = "none";
    } else {
        VecStr tokens = split(trimmedLine, ' ');
        if (tokens.size() >= 2)
        {
            key = tokens[0];
            value = tokens[1];

            std::string cutSemicolon;
            value = cutTillSemicolon(value);
            std::string KeyWithoutLastSection;
            if (trimmedLine[trimmedLine.size() - 1] == '\'')
                handleLogFormat(trimmedLine, value, tokens, it);
            else
                for (size_t i = 2; i < tokens.size(); ++i)
                    value += " " + cutTillSemicolon(tokens[i]);
        }
    }

    fillMap(value, key, currentSection, getKeyWithoutLastSection());
}

void ConfigDB::execParser(char *argv[])
{
    std::string configData;
    std::string currentSection = "";
    int start = 0;
    int end = 0;

    configData = this->readFile(argv);
    VecStr lines = split(configData, '\n');
    for (VecStr::const_iterator it = lines.begin(); it != lines.end(); ++it)
    {
        std::string trimmedLine = *it;
        size_t endSection = trimmedLine.find('}');
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t\n\r\f\v"));
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t\n\r\f\v") + 1);
        if (trimmedLine.empty())
            continue;
        if (trimmedLine[trimmedLine.size() - 1] == '{')
            currentSection = this->handleKeySection(start, end, trimmedLine);
        else if (endSection != std::string::npos)
            this->eraseLastSection();
        else {

            splitDirectiveAndValue(currentSection, it, trimmedLine);
        }
    }
    splitDB(_keyValues);
}

ConfigDB::KeyValues ConfigDB::getKeyValue()
{
    return this->_keyValues;
}

void ConfigDB::handleRootDB(MapStr keyMap, const std::string &key, const VecStr &values, size_t lhs, size_t rhs)
{
    if (rhs != std::string::npos)
        keyMap["directives"] = (rhs != std::string::npos)
                                   ? key.substr(rhs + 2)
                                   : key.substr(0, key.length() - 3);
    if (lhs != std::string::npos)
        keyMap["location"] = key.substr(0, lhs);
    groupedRootData[counter].push_back(std::make_pair(keyMap, values));
    counter++;
}

void ConfigDB::handleServerDB(MapStr keyMap, const std::string &key, const VecStr &values, size_t lhs, size_t rhs)
{
    std::string indexStr = key.substr(lhs + 1, rhs - lhs - 1);
    int index = atoi(indexStr.c_str());

    keyMap["directives"] = (rhs != std::string::npos)
                               ? key.substr(rhs + 2)
                               : key.substr(0, key.length() - 3);
    size_t locationStart = key.find("location_");
    if (locationStart != std::string::npos)
    {
        size_t locationEnd = key.find("[");
        if (locationEnd != std::string::npos)
            keyMap["location"] = key.substr(locationStart + 9, locationEnd - locationStart - 9);
    }
    groupedServers[index].push_back(std::make_pair(keyMap, values));
}

void ConfigDB::splitDB(const KeyValues &keyValues)
{
    KeyValues::const_iterator it;
    for (it = keyValues.begin(); it != keyValues.end(); ++it)
    {
        const std::string &key = it->first;
        const VecStr &values = it->second;
        size_t lhs = key.find("[");
        size_t rhs = key.find("].");
        MapStr keyMap;

        keyMap["directives"] = key;
        keyMap["location"] = "";
        if (lhs != std::string::npos && rhs != std::string::npos && lhs < rhs)
        {
            (key.find("server") != std::string::npos)
                ? handleServerDB(keyMap, key, values, lhs, rhs)
                : handleRootDB(keyMap, key, values, lhs, rhs);
        }
        else
        {
            rhs = key.find("]");
            if (rhs != std::string::npos)
            {
                keyMap["directives"] = key.substr(0, key.length() - 3);
                groupedRootData[counter].push_back(std::make_pair(keyMap, values));
                counter++;
            }
            else
            {
                std::cout << "UNEXPECTED KEY: " << key << "\n";
            }
        }
    }
}

void ConfigDB::printChoice(bool allRootData, int rootDataIdx, bool allServersData, int serverDataIdx, bool allConfig)
{
    if (allRootData)
    {
        std::cout << "**** ALL ROOT DATA"
                  << " ****" << std::endl;
        printAllDBData(groupedRootData);
        std::cout << std::endl;
    }
    if (rootDataIdx >= 0)
    {
        std::cout << "**** ROOT: " << rootDataIdx << " ****" << std::endl;
        printData(getDataByIdx(groupedRootData, rootDataIdx));
        std::cout << std::endl;
    }
    if (allServersData)
    {
        std::cout << "**** ALL SERVER DATA"
                  << " ****" << std::endl;
        printAllDBData(groupedServers);
        std::cout << std::endl;
    }
    if (serverDataIdx >= 0)
    {
        std::cout << "**** SERVER: " << serverDataIdx << " ****" << std::endl;
        printData(getDataByIdx(groupedServers, serverDataIdx));
        std::cout << std::endl;
    }
    if (allConfig)
    {
        std::cout << "**** ALL CONFIG DATA"
                  << " ****" << std::endl;
        printKeyValue();
        std::cout << std::endl;
    }
}

ConfigDB::GroupedDBMap ConfigDB::getServers() const
{
    return groupedServers;
}

ConfigDB::GroupedDBMap ConfigDB::getRootConfig() const
{
    return groupedRootData;
}
