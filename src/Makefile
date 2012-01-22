# Makefile for Asf - tools project

#################

GZIP_TOOL = gzip

PLAYER_TOOL = player

CXX = g++

CFLAGS = -Wall -g

ZLIB = -lz

INCLUDES = $(shell pkg-config --cflags opencv)

LIBS = $(shell pkg-config --libs opencv)

all : $(GZIP_TOOL) $(PLAYER_TOOL)

#################
## Gzip tool
#################
$(GZIP_TOOL): gzip.o Gzip.o
	$(CXX) -o $(GZIP_TOOL) gzip.o Gzip.o $(CFLAGS) $(ZLIB)

gzip.o: gzip.cpp
	$(CXX) -c gzip.cpp $(CFLAGS)

Gzip.o: Gzip.h Gzip.cpp
	$(CXX) -c Gzip.h Gzip.cpp $(CFLAGS)

#################
## Player tool
#################

$(PLAYER_TOOL): main.o AsfPlayer.o AsfFile.o
	$(CXX) -o $(PLAYER_TOOL) main.o AsfPlayer.o AsfFile.o $(CFLAGS) $(INCLUDES) $(LIBS)

main.o: main.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) main.cpp

AsfPlayer.o: AsfPlayer.h AsfPlayer.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) AsfPlayer.cpp AsfPlayer.h

AsfFile.o: AsfFile.h AsfFile.cpp
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(LIBS) AsfFile.cpp AsfFile.h

clean:
	rm -f *.o $(GZIP_TOOL) $(PLAYER_TOOL)