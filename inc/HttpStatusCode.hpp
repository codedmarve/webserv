/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatusCode.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/28 23:14:51 by alappas           #+#    #+#             */
/*   Updated: 2024/05/28 23:14:52 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSTATUSCODES_HPP
#define HTTPSTATUSCODES_HPP

#include "./AllHeaders.hpp"

class HttpStatusCodes {
private:
    std::map<int, std::string> codeMap;

public:
    HttpStatusCodes();
    ~HttpStatusCodes();

    HttpStatusCodes(const HttpStatusCodes &rhs);
    HttpStatusCodes &operator=(const HttpStatusCodes &rhs);
    
    std::string getStatusCode(int code);
};

#endif