CC=gcc
CFLAGS=-Wall -pthread
BIN=bin
INC=include
OBJ=obj
SRC=src

all: redchat

redchat.o:
	$(CC) $(CFLAGS) -c $(SRC)/redchat.c -o $(OBJ)/redchat.o

units.o:
	$(CC) $(CFLAGS) -c $(SRC)/units.c -o $(OBJ)/units.o

utils.o:
	$(CC) $(CFLAGS) -c $(SRC)/utils.c -o $(OBJ)/utils.o

redchat: units.o utils.o redchat.o
	$(CC) $(CFLAGS) $(OBJ)/units.o $(OBJ)/utils.o $(OBJ)/redchat.o \
		-o $(BIN)/redchat

clean:
	rm -vf $(OBJ)/*.o
	rm -vf $(BIN)/redchat
