/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sever.main.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/29 01:23:03 by alappas           #+#    #+#             */
/*   Updated: 2024/04/08 00:43:40 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

int serverMain(int argc, char **argv) {
      if (argc < 2)
        ft_errors(argv[0],1); 
    ConfigDB base;
    base.execParser(argv);    
    /**
     * @brief print your choice of data.
     * void ConfigDB::printChoice(bool allRootData, int rootDataIdx, bool allServersData, int serverDataIdx, bool allConfig)
     * @param allRootData true prints all root data
     * @param rootDataIdx prints root data at n index. where n >= 0
     * @param allServersData true prints all server data
     * @param rootDataIdx prints server data at n index. where n >= 0
     * 
     * @return NULL;
     */
    base.printChoice(false, -1, false, -1, false);
    Servers servers(base);

    return 0;
}
