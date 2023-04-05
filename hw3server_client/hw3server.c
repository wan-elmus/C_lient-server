
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <pthread.h>

#define MAX_CLIENTS 20
#define MAX_MESSAGE_LEN 250
#define SERVER_PORT 45000

int client_sockets[MAX_CLIENTS]; 

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *client_handler(void *arg)
{
    int sockfd = *(int *)arg;
    char buffer[MAX_MESSAGE_LEN + 1];
    int n;

    // Send welcome message to client
    if (write(sockfd, "START\n", 6) < 0)
    {
        error("ERROR writing to socket");
    }

    // Receive welcome message from client
    n = read(sockfd, buffer, MAX_MESSAGE_LEN);
    if (n <= 0 || strncmp(buffer, "\rHI\n", 6) != 0)
    {
        error("ERROR invalid welcome message from client");
    }
    // Send "prompt" message on a separate line
    // if (write(sockfd, "root@client:~#\n", 6) < 0)
    if (write(sockfd, "root@client:~#\n", 15) < 0)
    {
        error("ERROR writing to socket");
    }

    while (true)
{
    n = read(sockfd, buffer, MAX_MESSAGE_LEN);
    if (n <= 0)
    {
        // Client closed connection
        close(sockfd);
        printf("Client disconnected\n");

        // Notify other clients that this client has disconnected
        for (int j = 0; j < MAX_CLIENTS; j++)
        {
            if (client_sockets[j] != -1 && client_sockets[j] != sockfd)
            {
                if (write(client_sockets[j], "Client disconnected\n", 21) < 0)
                {
                    error("ERROR writing to socket");
                }
            }
        }

        pthread_exit(NULL);
    }
    else
    {
        // Check if received message is the disconnect message
        if (strncmp(buffer, "\rSTOP\n", 6) == 0)
        {
            // Close socket and exit thread
            close(sockfd);
            printf("Client disconnected\n");

            // Notify other clients that this client has disconnected
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (client_sockets[j] != -1 && client_sockets[j] != sockfd)
                {
                    if (write(client_sockets[j], "Client disconnected\n", 21) < 0)
                    {
                        error("ERROR writing to socket");
                    }
                }
            }

            pthread_exit(NULL);
        }
        else
        {
            // Relay message to all other active clients
            buffer[n] = '\0';
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
                if (client_sockets[j] != -1 && client_sockets[j] != sockfd)
                {
                    // Send message to other active clients
                    if (write(client_sockets[j], buffer, n) < 0)
                    {
                        error("ERROR writing to socket");
                    }
                }
            }
        }
    }
}

}

int main()
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    pthread_t threads[MAX_CLIENTS];
    int num_clients = 0;

    // Initialize client sockets array
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client_sockets[i] = -1;
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

    while (true)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0)
        {
            error("ERROR on accept");
        }

        // Add new client

        // Check if maximum number of clients have already connected
        if (num_clients == MAX_CLIENTS)
        {
            char msg [] =("Maximum number of clients reached. Connection refused.\n");
            close(newsockfd);
        }
        else
        {
            // Find empty slot in client sockets array
            int i;
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_sockets[i] == -1)
                {
                    client_sockets[i] = newsockfd;
                    num_clients++;
                    break;
                }
            }

            // Create new thread to handle client communication
            if (pthread_create(&threads[i], NULL, client_handler, &client_sockets[i]) != 0)
            {
                error("ERROR creating thread");
            }

            printf("Client connected: %s\n", inet_ntoa(cli_addr.sin_addr));
        }
    }

    close(sockfd);
    return 0;
}