SRC = ./src
OBJ = ./obj
BIN = ./bin
LIB = ./lib

.PHONY: all create_dirs lib bin run-srv run-cli clean

all: bin

create_dirs:
	mkdir -p $(OBJ)
	mkdir -p $(BIN)

lib: create_dirs
	gcc -c $(SRC)/socketutils.c -I $(LIB) -o $(OBJ)/socketutils.o
	gcc -c $(SRC)/usertutils.c -I $(LIB) -o $(OBJ)/userutils.o

bin: lib
	gcc $(SRC)/gchatsrv.c $(OBJ)/*.o -I $(LIB) -o $(BIN)/gchatsrv
	gcc $(SRC)/gchatcli.c $(OBJ)/*.o -I $(LIB) -o $(BIN)/gchatcli

run-srv:
	$(BIN)/gchatsrv

run-cli:
	$(BIN)/gchatcli

clean: 
	rm -f $(BIN)/*
	rm -f $(OBJ)/*

fresh: clean all
