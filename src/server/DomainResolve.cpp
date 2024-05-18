/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DomainResolve.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oduwoledare <oduwoledare@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/14 14:48:06 by alappas           #+#    #+#             */
/*   Updated: 2024/03/22 12:08:05 by oduwoledare      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/AllHeaders.hpp"

    IpAddressResolver::IpAddressResolver() {
        memset(&addressHints, 0, sizeof(addressHints)); // clear addressHints structure
        addressResults = NULL; // initialize addressResults to NULL
    }

    IpAddressResolver::~IpAddressResolver() {
        freeaddrinfo(addressResults); // frees the memory allocated by getaddrinfo
    }

    int IpAddressResolver::resolve(const char *hostname) {
        return getaddrinfo(hostname, NULL, &addressHints, &addressResults); // resolves the hostname to IP addresses
    }

    void IpAddressResolver::print() const {
        std::cout << "ip addresses:\n";
        for (struct addrinfo *result = addressResults; result != NULL; result = result->ai_next) {
            char buffer[INET6_ADDRSTRLEN]; // buffer to store the IP address string
            void *addr; // ptr to the address structure

            // if the address family is IPv4
            if (result->ai_family == AF_INET) {
				// we need to cast the address as a sockaddr_in structure to
            	// get the IP address, since ai_addr might be either
         		// sockaddr_in (IPv4) or sockaddr_in6 (IPv6)
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr; // cast the address to IPv4 format
                addr = &(ipv4->sin_addr); // get the address from the structure
                inet_ntop(result->ai_family, addr, buffer, sizeof(buffer)); // convert the address to a string
                std::cout << "ipv4: " << buffer << std::endl; // Print the IPv4 address
            } 
			else { // if address family is IPv6
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)result->ai_addr; // cast the address to IPv6 format
                addr = &(ipv6->sin6_addr); // get the address from the structure
                inet_ntop(result->ai_family, addr, buffer, sizeof(buffer)); // convert the address to a string
                std::cout << "ipv6: " << buffer << std::endl; // print the IPv6 address
            }
        }
    }
