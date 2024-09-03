#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;
constexpr unsigned int MSG_REPLY_LENGTH = 18;

int main(int argc, char *argv[]) {
    // 1. Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "open socket error" << std::endl;
        return 1;
    }

    // 2. Set socket options to reuse the address
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // 3. Bind the socket to an address
    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "bind error" << std::endl;
        close(sockfd);  // Make sure to close the socket before returning
        return 2;
    }

    // 4. Listen for incoming connections
    listen(sockfd, 5);
    std::cout << "Server is listening on port " << SERVER_PORT << "..." << std::endl;

    while (true) {
        // 5. Accept a new connection
        socklen_t clilen = sizeof(cli_addr);
        int incomingSock = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (incomingSock < 0) {
            std::cerr << "accept error" << std::endl;
            continue;  // Continue to accept new connections
        }

        std::cout << "Server: got connection from = "
                  << inet_ntoa(cli_addr.sin_addr)
                  << " and port = " << ntohs(cli_addr.sin_port)
                 << std::endl
                 
                 << std::endl;

        // 6. Send initial message to the client
        write(incomingSock, "You are connected!", MSG_REPLY_LENGTH);

        // 7. Loop to continuously read messages from the client
        while (true) {
            std::string buffer(MAX_BUFFER, 0);
            ssize_t n = read(incomingSock, &buffer[0], MAX_BUFFER - 1);
            if (n <= 0) {
                // If read returns 0, the client closed the connection
                break;
            }

            // Output the message received
            std::cout << "Got the message: " << buffer << std::endl << std::endl;

            // Optional: Echo the message back to the client
            write(incomingSock, buffer.c_str(), buffer.length());
        }

        // 8. Close the client socket
        close(incomingSock);
    
    }
    // 9. Close the listening socket (though this line will not be reached in this setup)
    close(sockfd);
    
    return 0;
}
