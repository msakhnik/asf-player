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

$(NAME): test.o AsfPlayer.o AsfFile.o
	$(CXX) -o $(NAME) test.o   $(CFLAGS) AsfFile.o AsfPlayer.o $(LIBS) $(TEST_LIBS)

test.o: test.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) test.cpp

AsfFile.o: $(VPATH)AsfFile.h $(VPATH)AsfFile.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS)  $(VPATH)AsfFile.cpp  $(VPATH)AsfFile.h

AsfPlayer.o: $(VPATH)AsfPlayer.h $(VPATH)AsfPlayer.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS)  $(VPATH)AsfPlayer.cpp  $(VPATH)AsfPlayer.h

clean:
	rm -f *.o $(NAME)
