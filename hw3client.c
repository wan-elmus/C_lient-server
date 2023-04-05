
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_MESSAGE_LEN 251
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 45000

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[MAX_MESSAGE_LEN];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket");
    }

    // Initialize server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(SERVER_PORT);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Error connecting to server");
    }

    // Receive welcome message from server
    memset(message, 0, MAX_MESSAGE_LEN);
    if (recv(sockfd, message, MAX_MESSAGE_LEN, 0) < 0) {
        error("Error receiving welcome message");
    }
    printf("Server: %s", message);

    // Send welcome message back to server
    strcpy(message, "\rHI\n");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        error("Error sending welcome message");
    }

    // Send and receive messages until disconnect
    while (1) {
        // Send message to server
        printf("root@client:~#\n");
        fgets(message, MAX_MESSAGE_LEN, stdin);
        if (send(sockfd, message, strlen(message), 0) < 0) {
            error("Error sending message");
        }

        // Receive message from server
        memset(message, 0, MAX_MESSAGE_LEN);
        if (recv(sockfd, message, MAX_MESSAGE_LEN, 0) < 0) {
            error("Error receiving message");
        }
        printf("Server: %s", message);

        // Check for disconnect message
        if (strcmp(message, "\rSTOP\n") == 0) {
            printf("Disconnecting...\n");
            break;
        }
    }
    // Close socket
    close(sockfd);

    return 0;
}
