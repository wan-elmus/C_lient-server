
## C_Server

This is a simple chat server written in C. The server listens for incoming client connections and relays messages sent by clients to all other connected clients. It supports up to 20 clients at a time.

## Prerequisites

This program requires a C compiler and the following libraries:

stdio.h
stdlib.h
string.h
stdbool.h
unistd.h
arpa/inet.h
sys/select.h
pthread.h

## Usage

The working server and client code are in the chat_server directory.

To compile the code, navigate to the directory containing the hw3server.c file and run the following command:

*gcc sever.c -lpthread -o server*

Or simply use the Makefile by typing *make* in the terminal, in the chat_server directory.

**To start the server:**

*./server*

By default, the server listens on port 45000. Clients can connect to the server using a Telnet client or a custom client program.

## Key takeaways

~ The maximum number of clients is set to 20 by default, but can be changed by modifying the MAX_CLIENTS constant.

~ The maximum message length is set to 250 by default, but can be changed by modifying the MAX_MESSAGE_LEN constant.

~ When a client disconnects, the server sends a notification to all other connected clients.

~ The client sockets array is stored in global scope so that it can be accessed by the client handler threads.
