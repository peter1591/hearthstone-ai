
CFLAGS += -I. -I../../include
CFLAGS += -O2 -Wall -std=c++11 -g -static
#CFLAGS += -DCHOOSE_WHERE_TO_PUT_MINION

all:
	g++ $(CFLAGS) test.cpp ../../lib/*.cpp

i:
	g++ $(CFLAGS) -DINTERACTIVE -DDEBUG test.cpp ../../lib/*.cpp
