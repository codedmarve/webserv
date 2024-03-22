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


/*
int inet_pton(int af, const char * src, void *dst);
“pton” stands for “presentation to network”

inet_ntop is a function used for converting a binary IP address representation into a human-readable string format. 
It stands for "Internet Network to Presentation" and is commonly used in networking applications to display IP addresses in a readable format.
*/

// Automatically Fill In the IP Address with getaddrinfo():
// allows us to supply a domain name (http://www.example.com) instead of an IP address:
//
// int getaddrinfo(const char *node, const char *service,
//                const struct addrinfo *hints,
//                struct addrinfo **res);
// node: the domain name or IP address
// service: the port number or service name
// hints: a pointer to a struct addrinfo that contains hints or filters for the results
// res: a pointer to a pointer to a struct addrinfo that will store the results
//
// struct addrinfo {
//     int              ai_flags; // AI_PASSIVE - socket we will create will be used to listen for and accept connections in the context of a socket
//     int              ai_family; // force an IPv4 address with AF_INET, IPv6 address with AF_INET6
//     int              ai_socktype; // SOCK_STREAM, which uses TCP, and SOCK_DGRAM, which uses UDP.
//     int              ai_protocol; // TCP for a stream socket, UDP for a datagram socket (or 0 for either)
//     size_t           ai_addrlen;
//     struct sockaddr *ai_addr; // pointer to a sockaddr_in or sockaddr_in6, which getaddrinfo() will fill out for us.
//     char            *ai_canonname;
//     struct addrinfo *ai_next; // a pointer to the next element in the linked list.
// };

// struct addrinfo {
// 	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
// 	int	ai_family;	/* PF_xxx */
// 	int	ai_socktype;	/* SOCK_xxx */
// 	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
// 	socklen_t ai_addrlen;	/* length of ai_addr */
// 	char	*ai_canonname;	/* canonical name for hostname */
// 	struct	sockaddr *ai_addr;	/* binary address */
// 	struct	addrinfo *ai_next;	/* next structure in linked list */
// };



    // + frees the memory allocated for addressResults
    

    // resolves the IP address of the given hostname
    

    // prints the resolved IP addresses
    

// struct sockaddr_in {
// 	__uint8_t       sin_len;
// 	sa_family_t     sin_family;
// 	in_port_t       sin_port;
// 	struct  in_addr sin_addr;
// 	char            sin_zero[8];
// };

// struct sockaddr_in6 {
// 	__uint8_t       sin6_len;       /* length of this struct(sa_family_t) */
// 	sa_family_t     sin6_family;    /* AF_INET6 (sa_family_t) */
// 	in_port_t       sin6_port;      /* Transport layer port # (in_port_t) */
// 	__uint32_t      sin6_flowinfo;  /* IP6 flow information */
// 	struct in6_addr sin6_addr;      /* IP6 address */
// 	__uint32_t      sin6_scope_id;  /* scope zone index */
// };
