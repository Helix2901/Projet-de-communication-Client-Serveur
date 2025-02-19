
CC = gcc
CFLAGS = -Wall -Wextra -g -pthread


# Directories
CLIENT_DIR = ./client
SERVEUR_DIR = ./serveur

# Executable files
CLIENT_EXEC = client_exec
SERVEUR_EXEC = serveur_exec

# Source files
CLIENT_SRC = $(CLIENT_DIR)/client.c
CLIENT_OBJ = $(CLIENT_DIR)/client.o

SERVEUR_SRC = $(SERVEUR_DIR)/serveur.c
SERVEUR_OBJ = $(SERVEUR_DIR)/serveur.o

# Rules
all: $(CLIENT_EXEC) $(SERVEUR_EXEC)

$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJ)

$(SERVEUR_EXEC): $(SERVEUR_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SERVEUR_OBJ)

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $(CLIENT_SRC) -o $(CLIENT_OBJ)

$(SERVEUR_OBJ): $(SERVEUR_SRC)
	$(CC) $(CFLAGS) -c $(SERVEUR_SRC) -o $(SERVEUR_OBJ)

clean:
	rm -f $(CLIENT_EXEC) $(SERVEUR_EXEC) $(CLIENT_OBJ) $(SERVEUR_OBJ)

.PHONY: clean all
