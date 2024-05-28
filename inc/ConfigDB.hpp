/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigDB.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:14:29 by alappas           #+#    #+#             */
/*   Updated: 2024/05/28 23:14:31 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ConfigDB_H
# define ConfigDB_H

#include "AllHeaders.hpp"

class ConfigDB{
	public:
		typedef std::map<std::string, std::string> MapStr;
		typedef std::vector<std::string> VecStr;
	    typedef std::map<std::string, VecStr > KeyValues;
        typedef std::pair<MapStr, VecStr > KeyMapValue;
        typedef std::map<int, std::vector<KeyMapValue > > GroupedDBMap;

		ConfigDB();
		~ConfigDB();
		ConfigDB(const ConfigDB &rhs);
		ConfigDB &operator=(const ConfigDB &rhs);
		
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
		void splitDB(const KeyValues& keyValues);
		void splitDirectiveAndValue(std::string currentSection, VecStr::const_iterator it, std::string trimmedLine);
		void printChoice(bool allRootData, int rootDataIdx, bool allServersData, int serverDataIdx, bool allConfig);
		void handleRootDB(MapStr keyMap, const std::string& key,const VecStr& values, size_t lhs, size_t rhs);
		void handleServerDB(MapStr keyMap, const std::string& key,const VecStr& values, size_t lhs, size_t rhs);
		GroupedDBMap getServers() const;
		GroupedDBMap getRootConfig() const;

		
	private:
		VecStr _variablePath;
		std::map<std::string, int> sectionCounts;
		KeyValues _keyValues;
		GroupedDBMap groupedServers;
		GroupedDBMap groupedRootData;
		size_t counter;
};

#endif
