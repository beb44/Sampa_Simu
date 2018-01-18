CC = g++
EXEC_NAME = main.out 
CFLAGS = -std=c++0x -Wall -g -pthread 
EXEC_NAME = ../bin/Sampa_simu.out 
LIB_NAME = ../lib/Sampa_simu.a 
INCLUDES =
LIBS = -lstdc++ -lpthread
SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=../obj/%.o) 
INSTALL_DIR = /usr/bin

all : $(EXEC_NAME)

clean :
	rm $(EXEC_NAME) $(OBJ_FILES)

lib:
	ar rcs $(LIB_NAME) $(OBJ_FILES)

$(EXEC_NAME) : $(OBJ_FILES)
	$(CC) -o $(EXEC_NAME) $(OBJ_FILES) $(LIBS)

../obj/%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

../obj/%.o: %.cc
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

../obj/%.o: %.c
	gcc $(CFLAGS) $(INCLUDES) -o $@ -c $<
