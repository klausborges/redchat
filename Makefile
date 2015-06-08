CC=gcc
CFLAGS=-Wall -pthread
BIN=bin
INC=include
OBJ=obj
SRC=src

all: redchat

redchat.o:
	$(CC) $(CFLAGS) -c $(SRC)/redchat.c -o $(OBJ)/redchat.o

client.o:
	$(CC) $(CFLAGS) -c $(SRC)/client.c -o $(OBJ)/client.o

server.o:
	$(CC) $(CFLAGS) -c $(SRC)/server.c -o $(OBJ)/server.o

interact.o:
	$(CC) $(CFLAGS) -c $(SRC)/interact.c -o $(OBJ)/interact.o

utils.o:
	$(CC) $(CFLAGS) -c $(SRC)/utils.c -o $(OBJ)/utils.o

redchat: interact.o client.o server.o utils.o redchat.o
	$(CC) $(CFLAGS) $(OBJ)/interact.o $(OBJ)/client.o $(OBJ)/server.o \
		$(OBJ)/utils.o $(OBJ)/redchat.o -o $(BIN)/redchat

clean:
	rm -vf $(OBJ)/*.o
	rm -vf $(BIN)/redchat
