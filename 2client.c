
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

    // Send greeting message to server
    strcpy(message, "Hi, how are you doing?\n");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        error("Error sending message");
    }

    // Receive response from server
    memset(message, 0, MAX_MESSAGE_LEN);
    if (recv(sockfd, message, MAX_MESSAGE_LEN, 0) < 0) {
        error("Error receiving message");
    }
    printf("Server: %s", message);

    // Send another message to server
    strcpy(message, "I'm good too, thanks. What are you up to?\n");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        error("Error sending message");
    }

    // Receive response from server
    memset(message, 0, MAX_MESSAGE_LEN);
    if (recv(sockfd, message, MAX_MESSAGE_LEN, 0) < 0) {
        error("Error receiving message");
    }
    printf("Server: %s", message);

    // Send another message to server
    strcpy(message, "I'm just testing this chat application. It seems to work well.\n");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        error("Error sending message");
    }

    // Receive response from server
    memset(message, 0, MAX_MESSAGE_LEN);
    if (recv(sockfd, message, MAX_MESSAGE_LEN, 0) < 0) {
        error("Error receiving message");
    }
    printf("Server: %s", message);

    // Send a farewell message to server
    strcpy(message, "Sure, will do. Bye!\n");
    if (send(sockfd, message, strlen(message), 0) < 0) {
        error("Error sending message");
// Close socket
    close(sockfd);

    return 0;
}
