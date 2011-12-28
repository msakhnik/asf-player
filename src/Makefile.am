bin_PROGRAMS = asf_player

asf_player_SOURCES = AsfPlayer.cpp AsfPlayer.h \
                AsfFile.cpp AsfFile.h \
                main.cpp
CXX = g++
CFLAGS = -Wall -g
INCLUDES = $(shell pkg-config --cflags opencv)
LIBS = $(shell pkg-config --libs opencv)
