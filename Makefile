CXX		  := g++
# CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb
CXX_FLAGS := -Wall -Wextra -std=c++17

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

OPENCV = `pkg-config opencv --cflags --libs`
LIBRARIES	= $(OPENCV)
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
