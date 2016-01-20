
CFLAGS += -I. -I../../include
CFLAGS += -O2 -Wall -Wpedantic -Wextra -std=c++11 -g -static
CFLAGS += -DCHOOSE_WHERE_TO_PUT_MINION

all:
	g++ $(CFLAGS) test.cpp ../../lib/*.cpp

.PHONY: i
i:
	g++ $(CFLAGS) -DINTERACTIVE -DDEBUG test.cpp ../../lib/*.cpp
