
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 20
#define MAX_MSG_LEN 250

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen, n;
    char buffer[MAX_MSG_LEN+1];
    struct sockaddr_in serv_addr, cli_addr;
    int client_fds[MAX_CLIENTS];
    int num_clients = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);

    printf("Server running on port %d\n", portno);

    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Add new client to list
        if (num_clients < MAX_CLIENTS) {
            client_fds[num_clients] = newsockfd;
            num_clients++;
            printf("New client connected. Total clients: %d\n", num_clients);

            // Send welcome message to client
            n = write(newsockfd, "START\n", 6);
            if (n < 0)
                error("ERROR writing to socket");
        } else {
            // Too many clients - reject connection
            printf("Too many clients. Connection rejected.\n");
            close(newsockfd);
            continue;
        }

        // Wait for welcome message from client
        memset(buffer, 0, MAX_MSG_LEN+1);
        n = read(newsockfd, buffer, MAX_MSG_LEN);
        if (n < 0)
            error("ERROR reading from socket");

        if (strcmp(buffer, "\rHI\n") != 0) {
            // Invalid welcome message - disconnect client
            printf("Invalid welcome message. Disconnecting client.\n");
            close(newsockfd);
            continue;
        }

        // Relay messages from client to other clients
        while (1) {
            memset(buffer, 0, MAX_MSG_LEN+1);
            n = read(newsockfd, buffer, MAX_MSG_LEN);
            if (n < 0)
                error("ERROR reading from socket");
            if (strcmp(buffer, "\rSTOP\n") == 0) {
                // Client disconnected - remove from list
                printf("Client disconnected. Total clients: %d\n", num_clients-1);
                close(newsockfd);
                for (int i = 0; i < num_clients; i++) {
                    if (client_fds[i] == newsockfd) {
                        for (int j = i; j < num_clients-1; j++) {
                            client_fds[j] = client_fds[j+1];
                        }
                    num_clients--;
                    break;
                    }
                }
            break;
            }
        // Relay message to all other clients
                for (int i = 0; i < num_clients; i++) {
                    if (client_fds[i] != newsockfd) {
                        n= write(client_fds[i], buffer, strlen(buffer));
                        if (n < 0)
                            error("ERROR writing to socket");
                    }           
                }
        }
    }   
    close(sockfd);
    return 0;
}
