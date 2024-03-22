/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DomainResolve.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alappas <alappas@student.42wolfsburg.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 14:51:19 by alappas           #+#    #+#             */
/*   Updated: 2024/03/14 14:56:31 by alappas          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AllHeaders.hpp"

class IpAddressResolver {
private:
    struct addrinfo addressHints; // provide hints for the address resolution
    struct addrinfo *addressResults; // store results of the address resolution

public:
    IpAddressResolver();
    ~IpAddressResolver();
    int resolve (const char *hostname);
    void print() const;
};