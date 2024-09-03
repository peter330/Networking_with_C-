#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>

constexpr unsigned int SERVER_PORT = 50544;
constexpr unsigned int MAX_BUFFER = 128;

int main(int argc, char *argv[])
{
    while(true){
        // 1. Create the socket
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
        std::cerr << "socket error" << std::endl;
        return 1;
        }

        // 2. Get the server information
        struct hostent* server = gethostbyname(argv[1]);
        if (server == nullptr)
        {
        std::cerr << "gethostbyname, no such host" << std::endl;
        return 2;
        }
        
        // 3. Initialize server address structure
        struct sockaddr_in serv_addr;
        bzero((char *) &serv_addr, sizeof(serv_addr));

        // IPV4.
        serv_addr.sin_family = AF_INET;

        // copy struct to struct.
        bcopy((char *)server->h_addr,
        (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

        // SOCKET ---> Network order.
        serv_addr.sin_port = htons(SERVER_PORT);

        // 4. Connect to the server
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof
        (serv_addr)) < 0)
        {
        std::cerr << "connect error" << std::endl;
        return 3;
        }
        // 5. send and Receive messages from the server
        std::string readBuffer (MAX_BUFFER, 0);
        if (read(sockfd, &readBuffer[0], MAX_BUFFER-1) < 0)
        {
        std::cerr << "read from socket failed" << std::endl;
        return 5;
        }
        std::cout << readBuffer << std::endl;
        std::string writeBuffer (MAX_BUFFER, 0);
        std::cout << "What message for the server? : ";
        getline(std::cin, writeBuffer);

        if (write(sockfd, writeBuffer.c_str(), strlen(writeBuffer.c_str())) < 0)
        {
        std::cerr << "write to socket" << std::endl;
        return 4;
        }

         // 6. Close the socket after exiting the loop
        close(sockfd);
    }

return 0;
}