/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DataBase.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oduwoledare <oduwoledare@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/09 11:16:08 by nikitos           #+#    #+#             */
/*   Updated: 2024/03/31 10:12:38 by oduwoledare      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DATABASE_H
# define DATABASE_H

#include "AllHeaders.hpp"

class DataBase{
	public: 
		DataBase();
		~DataBase();
		void		pushInBase(std::string env_name);
		void		eraseLastSection();
		void		fillMap(std::string value, std::string key, std::string currentSection ,std::string KeyWithoutLastSection);
		void		printKeyValue();
		void		execParser(char *argv[]);
		std::string	handleKeySection(int &start, int &end, std::string &line);
		std::string	readFile(char **argv);
		std::string	getFullPathKey();
		std::string getKeyWithoutLastSection();
		std::map<std::string, std::vector<std::string> > getKeyValue();
		void groupValuesByIndex(const std::map<std::string, std::vector<std::string> >& keyValues);
		std::map<int, std::vector<std::pair<std::string,std::vector<std::string> > > > groupedValues;
		void printValuesAtIndex(int index, const std::map<int, std::vector<std::pair<std::string, std::vector<std::string> > > >& groupedValues);
	private:
		std::vector<std::string> _variablePath;
		std::map<std::string, int> sectionCounts;
		std::map<std::string, std::vector<std::string> > _keyValues;
};

#endif
