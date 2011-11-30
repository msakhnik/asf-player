
SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

MAIN = player

##########################################################################

CXX = g++

CFLAGS = -Wall -g

INCLUDES = $(shell pkg-config --cflags opencv)

LIBS = $(shell pkg-config --libs opencv)

##########################################################################

.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

-include $(OBJS:%.o=.deps/%.d)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@
	@[ -d .deps ] || mkdir .deps
	@$(CXX) -MM $(CFLAGS) $(INCLUDES) $< > .deps/$*.d

clean:
	rm -rf *.o *~ $(MAIN) .deps

