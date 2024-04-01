/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigDB.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: drey <drey@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 11:16:08 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/31 18:12:55 by drey             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ConfigDB_H
# define ConfigDB_H

#include "AllHeaders.hpp"

class ConfigDB{
	public:
	    typedef std::map<std::string, std::vector<std::string> > KeyValues;
        typedef std::pair<std::map<std::string, std::string>, std::vector<std::string> > KeyMapValue;
        typedef std::map<int, std::vector<KeyMapValue > > GroupedValuesMap;

		ConfigDB();
		~ConfigDB();
		void		pushInBase(std::string env_name);
		void		eraseLastSection();
		void		fillMap(std::string value, std::string key, std::string currentSection ,std::string KeyWithoutLastSection);
		void		printKeyValue();
		void		execParser(char *argv[]);
		std::string	handleKeySection(int &start, int &end, std::string &line);
		std::string	readFile(char **argv);
		std::string	getFullPathKey();
		std::string getKeyWithoutLastSection();
		KeyValues getKeyValue();
		void groupValuesByIdx(const KeyValues& keyValues);
		void printAllServersData();
		std::vector<ConfigDB::KeyMapValue> getServerDataByIdx(int index);
		void printServerData(const std::vector<ConfigDB::KeyMapValue>& values);

		
	private:
		std::vector<std::string> _variablePath;
		std::map<std::string, int> sectionCounts;
		KeyValues _keyValues;
		KeyValues _HttpDirectives;
		GroupedValuesMap groupedValues;
};

#endif
