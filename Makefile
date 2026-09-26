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

clean: 
	rm -f $(BIN)/*
	rm -f $(OBJ)/*

fresh: clean all
