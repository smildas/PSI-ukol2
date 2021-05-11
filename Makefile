CC = gcc
CFLAGS = -Wall -pedantic -ansi
BIN = simpleHttp
OBJ = main.o 

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@ -lpthread

$(BIN): $(OBJ)
	$(CC) $^ -o $@ -lpthread

