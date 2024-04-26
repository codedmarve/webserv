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
    
    // std::map<std::string, std::vector<std::string> > keyValues = base.getKeyValue();
    // for(std::map<std::string, std::vector<std::string> >::iterator it = keyValues.begin(); it != keyValues.end(); it++)
    // {
    //     std::cout << it->first << " : ";
    //     for(std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
    //         std::cout << *it2 << " ";
    //     std::cout << std::endl;
    // }
    // IpAddressResolver resolver; // create an instance of the IpAddressResolver class

    // // // resolve the IP address of the given hostname
    // if (resolver.resolve("127.0.0.1:8001") != 0) {
    //     std::cerr << "getaddrinfo: " << gai_strerror(errno) << std::endl;
    //     return 2;
    // }

    // // print the resolved IP addresses
    // resolver.print();
    // (void)argc;
    // (void)argv;
    return 0;
}

//Socket creation template
// int main() {
    // // Create socket
    // int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // if (server_fd == -1) {
    //     std::cerr << "Socket creation failed\n";
    //     return 1;
    // }

    // // Bind socket
    // struct sockaddr_in address;
    // int addrlen = sizeof(address);
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(8080); // Example port
    // if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    //     std::cerr << "Bind failed\n";
    //     return 1;
    // }
    // // Listen
    // if (listen(server_fd, 3) == -1) {
    //     std::cerr << "Listen failed\n";
    //     return 1;
    // }

    // // Accept connections
    // int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    // if (new_socket == -1) {
    //     std::cerr << "Accept failed\n";
    //     return 1;
    // }
    
    //  // Send data
    // const char *message = "Hello, client!";
    // int bytes_sent = send(new_socket, message, strlen(message), 0);
    // if (bytes_sent == -1) {
    //     std::cerr << "Send failed\n";
    // } else {
    //     std::cout << "Data sent successfully\n";
    // }
    
    // // Handle connection (you can send/receive data here)

    // // Close sockets
    // close(new_socket);
    // close(server_fd);

//     return 0;
// }

// Epoll template

// #include <sys/epoll.h>
// #include <fcntl.h>
// #include <vector>

// std::vector<int> server_fds; // Assume this vector contains your server file descriptors

// // Create an epoll instance
// int epoll_fd = epoll_create1(0);
// if (epoll_fd == -1) {
//     // handle error
// }

// // Add each server file descriptor to the epoll instance
// for (int server_fd : server_fds) {
//     // Make the socket non-blocking
//     int flags = fcntl(server_fd, F_GETFL, 0);
//     if (flags == -1) {
//         // handle error
//     }
//     if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
//         // handle error
//     }

//     // Add the server file descriptor to the epoll instance
//     struct epoll_event event;
//     event.events = EPOLLIN; // Monitor for incoming data
//     event.data.fd = server_fd;
//     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
//         // handle error
//     }
// }

// // Main event loop
// while (true) {
//     struct epoll_event events[10]; // Array to hold returned events
//     int num_events = epoll_wait(epoll_fd, events, 10, -1); // Wait indefinitely for an event
//     if (num_events == -1) {
//         // handle error
//     }

//     for (int i = 0; i < num_events; i++) {
//         if (events[i].events & EPOLLIN) {
//             // There's incoming data on events[i].data.fd
//             // Read the data and handle it
//         }
//     }
// // }