# Makefile for Asf - tools project

#################

NAME = test

CXX = g++

CFLAGS = -Wall -g

ZLIB = -lz

INCLUDES = $(shell pkg-config --cflags opencv)

LIBS = $(shell pkg-config --libs opencv)

TEST_LIBS = $(shell gtest-config --ldflags --libs)

VPATH = ../src/

all : $(NAME)

#################

$(NAME): test.o $(VPATH)AsfPlayer.o $(VPATH)AsfFile.o
	$(CXX) -o $(NAME) test.o $(VPATH)AsfPlayer.o $(VPATH)AsfFile.o $(CFLAGS) $(INCLUDES) $(LIBS) $(TEST_LIBS)

test.o: test.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) test.cpp

AsfPlayer.o: $(VPATH)AsfPlayer.h $(VPATH)AsfPlayer.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) AsfPlayer.cpp AsfPlayer.h

AsfFile.o: ../src/AsfFile.h ../src/AsfFile.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) AsfFile.cpp AsfFile.h

clean:
	rm -f *.o $(NAME)
