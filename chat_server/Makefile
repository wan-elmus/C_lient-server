
CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -lpthread
SERVER_SOURCES = hw3server.c
CLIENT_SOURCES = client.c
SERVER_EXECUTABLE = server
CLIENT_EXECUTABLE = client

all: $(SERVER_EXECUTABLE) $(CLIENT_EXECUTABLE)

$(SERVER_EXECUTABLE): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT_EXECUTABLE): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(SERVER_EXECUTABLE) $(CLIENT_EXECUTABLE)

