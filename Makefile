SRC = ./src
OBJ = ./obj
BIN = ./bin
LIB = ./lib

CFLAGS = -g -Wall -Wextra -I $(LIB)

.PHONY: all create_dirs lib bin run-srv run-cli clean debug-srv debug-cli

all: bin

create_dirs:
	mkdir -p $(OBJ)
	mkdir -p $(BIN)

lib: create_dirs
	gcc $(CFLAGS) -c $(SRC)/socketutils.c -o $(OBJ)/socketutils.o
	gcc $(CFLAGS) -c $(SRC)/userutils.c -o $(OBJ)/userutils.o
	gcc $(CFLAGS) -c $(SRC)/generalutils.c -o $(OBJ)/generalutils.o
	gcc $(CFLAGS) -c $(SRC)/srvutils.c -o $(OBJ)/srvutils.o
	gcc $(CFLAGS) -c $(SRC)/cliutils.c -o $(OBJ)/cliutils.o

bin: lib
	gcc $(CFLAGS) $(SRC)/gchatsrv.c $(OBJ)/*.o -o $(BIN)/gchatsrv
	gcc $(CFLAGS) $(SRC)/gchatcli.c $(OBJ)/*.o -o $(BIN)/gchatcli

run-srv:
	$(BIN)/gchatsrv

run-cli:
	$(BIN)/gchatcli

debug-srv:
	gdb $(BIN)/gchatsrv

debug-cli:
	gdb $(BIN)/gchatcli

valgrind-srv:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/gchatsrv

valgrind-cli:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BIN)/gchatcli

clean: 
	rm -f $(BIN)/*
	rm -f $(OBJ)/*

fresh: clean all
