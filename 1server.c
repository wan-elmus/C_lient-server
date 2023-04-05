
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define MAX_CLIENTS 20
#define MAX_MESSAGE_LEN 250
#define SERVER_PORT 45000

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main()
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[MAX_MESSAGE_LEN + 1];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    int client_sockets[MAX_CLIENTS];
    int num_clients = 0;
    bool active_clients[MAX_CLIENTS];
    fd_set read_fds, master_fds;

    // Initialize active clients array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        active_clients[i] = false;
    }

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }

    // Initialize server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = SERVER_PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
    }

    // Listen for incoming connections
    listen(sockfd, MAX_CLIENTS);

    printf("Server is running and listening on port %d...\n", portno);

    FD_ZERO(&master_fds);
    FD_SET(sockfd, &master_fds);
    int fdmax = sockfd;

    while (true)
    {
        read_fds = master_fds;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            error("ERROR in select");
        }

        for (int i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == sockfd)
                {
                    // New connection request
                    clilen = sizeof(cli_addr);
                    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
                    if (newsockfd < 0)
                    {
                        error("ERROR on accept");
                    }

                    // Add new client socket to master file descriptor set
                    FD_SET(newsockfd, &master_fds);
                    if (newsockfd > fdmax)
                    {
                        fdmax = newsockfd;
                    }

                    // Send welcome message to new client
                    n = write(newsockfd, "START\n", 6);
                    if (n < 0)
                    {
                        error("ERROR writing to socket");
                    }

                    printf("Client connected: %s\n", inet_ntoa(cli_addr.sin_addr));
                    num_clients++;
                }
                else
                {
                    // Data from existing client
                    n = read(i, buffer, MAX_MESSAGE_LEN);
                    if (n <= 0)
                    {
                        // Client closed connection
                        close(i);
                        FD_CLR(i, &master_fds);
                        active_clients[i] = false;
                        printf("Client disconnected\n");
                        num_clients--;
                    }
                    else
                    {
                        // Relay message to all other active clients
                        buffer[n] = '\0';
                        for (int j = 0; j <= fdmax; j++)
                        {
                            if (FD_ISSET(j, &master_fds) && j != sockfd && j != i)
                            {

                            // Send message to other active clients
                            int sent = write(j, buffer, strlen(buffer));
                                if (sent < 0)
                                {
                                    error("ERROR writing to socket");
                                }
                            }
                        }
                    }
                }
            }
        }       
    }
    // Close all client sockets
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (active_clients[i])
        {
        close(client_sockets[i]);
        }
    }   

    // Close server socket
    close(sockfd);
    return 0;
}
