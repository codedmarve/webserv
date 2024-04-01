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

ConfigDB::ConfigDB(){}

ConfigDB::~ConfigDB(){}

void	ConfigDB::pushInBase(std::string env_name)
{
    this->_variablePath.push_back(env_name);
}

void	ConfigDB::eraseLastSection()
{
    if(!this->_variablePath.empty()) // Probably we don't need that condition
        this->_variablePath.pop_back();
}

std::string	ConfigDB::getFullPathKey()
{
	std::vector<std::string>::iterator it;
	std::string finalKey;

	for(it = _variablePath.begin(); it != _variablePath.end(); it++)
        finalKey += *it + ".";
	return finalKey;
}

std::string ConfigDB::getKeyWithoutLastSection()
{
    std::vector<std::string>::iterator it;
    std::vector<std::string> tmp = this->_variablePath;
	std::string finalKey;

    if(!tmp.empty())
    {
        tmp.pop_back();
    }
	for(it = tmp.begin(); it != tmp.end(); it++)
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
        ft_errors(configFile,2);

    while (std::getline(file, line)) {
        std::size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v")); // Trim leading whitespace
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1); // Trim trailing whitespace

        // If the line ends with a space, concatenate with the next line
        if (!line.empty() && line[line.size() - 1] == ' ') {
            lastLine += line.substr(0, line.size() - 1) + " ";
        } else {
            if (!lastLine.empty()) {
                lastLine += line;
                configData += lastLine + "\n";
                lastLine.clear();
            } else {
                configData += line + "\n";
            }
        }
    }
    if (!lastLine.empty()) {
        configData += lastLine;
    }
    if(checkCurly(configData))
        ft_errors("curly ", 3);
    return configData;
}

std::string		ConfigDB::handleKeySection(int &start, int &end, std::string &line)
{
    std::string currentSection = "";

    trimWordFromEnd(start, end, line);
    currentSection = line.substr(start, end + 1);
    std::replace(currentSection.begin(), currentSection.end(), ' ', '_');
    this->pushInBase(currentSection);
    if (sectionCounts.find(currentSection) == sectionCounts.end()) {
        sectionCounts[currentSection] = 0;
    } else {
        sectionCounts[currentSection]++;
    }
    if (sectionCounts[currentSection] >= 0) {
        std::stringstream ss;
        ss << "[" << sectionCounts[currentSection] << "]";
        currentSection += ss.str();
    }
    return currentSection;
}

void ConfigDB::printKeyValue() {
    typedef std::map<std::string, std::vector<std::string> >::const_iterator MapIterator;

    for (MapIterator it1 = _keyValues.begin(); it1 != _keyValues.end(); ++it1) {
        std::cout << "Key: " << it1->first
        << "\nValue(s): " << std::endl;
        const std::vector<std::string>& values = it1->second;
        for (std::vector<std::string>::const_iterator it2 = values.begin(); it2 != values.end(); ++it2) {
            std::cout << "  " << *it2 << std::endl;
        }
        std::cout << "\n";
    }
}

void ConfigDB::fillMap(std::string value, std::string key, std::string currentSection ,std::string KeyWithoutLastSection)
{
    std::vector<std::string> splitedValue = customSplit(value, ' ');
    std::vector<std::string>::iterator spltValIt = splitedValue.begin();

    while (spltValIt != splitedValue.end())
    {
        if (!currentSection.empty()) {
            _keyValues[KeyWithoutLastSection + currentSection + "." + key].push_back(*spltValIt);
        } else {
            std::string indexKey = getIndexVariableKey(key, _keyValues);
            while (spltValIt != splitedValue.end()){
                _keyValues[key + indexKey].push_back(*spltValIt);
            spltValIt++;
            }
            break;
        }
            spltValIt++;
        }
}

void   ConfigDB::execParser(char *argv[]){
    std::string configData;
    std::string currentSection = "";
    std::map<std::string, int> sectionCounts;
    int start = 0;
    int end = 0;

    configData = this->readFile(argv);
    std::vector<std::string> lines = customSplit(configData, '\n');
    for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string trimmedLine = *it;
        size_t endSection = trimmedLine.find('}');
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t\n\r\f\v"));
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t\n\r\f\v") + 1);
        if (trimmedLine.empty()) {
            continue; 
        }
        if (trimmedLine[trimmedLine.size() - 1] == '{') {
            currentSection = this->handleKeySection(start, end, trimmedLine);
        }
        else if (endSection != std::string::npos) {
            this->eraseLastSection();
        }
        else {
            std::vector<std::string> tokens = customSplit(trimmedLine, ' ');
            if (tokens.size() >= 2) {
                std::string key = tokens[0];
                std::string value = tokens[1];
                std::string cutSemicolon;
                value = cutTillSemicolon(value);
                std::string KeyWithoutLastSection;
                if (trimmedLine[trimmedLine.size() - 1] == '\''){
                    handleLogFormat(trimmedLine, value, tokens, it);
                }
                else{
                    for (size_t i = 2; i < tokens.size(); ++i) {
                        cutSemicolon = cutTillSemicolon(tokens[i]);
                        value += " " + cutSemicolon;
                    }
                }
            KeyWithoutLastSection = this->getKeyWithoutLastSection();
            this->fillMap(value, key, currentSection, KeyWithoutLastSection);
            }
        }
    }
}

ConfigDB::KeyValues ConfigDB::getKeyValue()
{
    return this->_keyValues;
}

void ConfigDB::groupValuesByIdx(const KeyValues& keyValues) {
    KeyValues::const_iterator it;
    for (it = keyValues.begin(); it != keyValues.end(); ++it) {
        const std::string& key = it->first;
        const std::vector<std::string>& values = it->second;

        size_t indexStart = key.find("[");
        size_t indexEnd = key.find("]");
        if (indexStart != std::string::npos && indexEnd != std::string::npos && indexStart < indexEnd) {
            std::string indexStr = key.substr(indexStart + 1, indexEnd - indexStart - 1);
            int index = atoi(indexStr.c_str());

            std::map<std::string, std::string> keyMap;
            keyMap["renamedKey"] = key;
            keyMap["location"] = "";

            size_t pos = key.find("].");
            if (pos != std::string::npos) {
                keyMap["renamedKey"] = key.substr(pos + 2);
            }

            size_t locationStart = key.find("location_");
            if (locationStart != std::string::npos) {
                size_t locationEnd = key.find("[");
                if (locationEnd != std::string::npos) {
                    keyMap["location"] = key.substr(locationStart + 9, locationEnd - locationStart - 9);
                }
            }

            groupedValues[index].push_back(std::make_pair(keyMap, values));
        }
    }
}

void ConfigDB::printAllServersData() {
    GroupedValuesMap::const_iterator it;
    for (it = groupedValues.begin(); it != groupedValues.end(); ++it) {
        std::cout << "Index: " << it->first << std::endl;
        for (size_t i = 0; i < it->second.size(); ++i) {
            const std::map<std::string, std::string>& keyMap = it->second[i].first;
            const std::vector<std::string>& values = it->second[i].second;

            std::cout << "{ " << keyMap.find("renamedKey")->second << ", "  << keyMap.find("location")->second << " }" << ": ";
            for (size_t j = 0; j < values.size(); ++j) {
                std::cout << "  " << values[j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "\n" << std::endl;
    }
}

void ConfigDB::printServerData(const std::vector<ConfigDB::KeyMapValue>& values) {
    for (size_t i = 0; i < values.size(); ++i) {
        const std::map<std::string, std::string>& keyMap = values[i].first;
        const std::vector<std::string>& valueVector = values[i].second;

        std::cout << "{ ";
        for (std::map<std::string, std::string>::const_iterator it = keyMap.begin(); it != keyMap.end(); ++it) {
            std::cout << it->first << ", " << it->second << ":";
        }

        for (size_t j = 0; j < valueVector.size(); ++j) {
            std::cout << "  " << valueVector[j];
        }
        std::cout  << std::endl;
    }
}




std::vector<ConfigDB::KeyMapValue> ConfigDB::getServerDataByIdx(int index) {
    std::vector<ConfigDB::KeyMapValue> values;
    GroupedValuesMap::iterator it = groupedValues.find(index);
    if (it != groupedValues.end()) {
        return it->second;
    }
    return values;
}



