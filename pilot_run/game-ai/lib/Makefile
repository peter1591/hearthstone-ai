include ../../Makefile.inc

CFLAGS = $(CFLAGS_PROJECT) -I../include -I../../game-engine/include
LIBS = ../../game-engine/lib/libGameEngine.a

all:
	g++ main.cpp mcts.cpp $(CFLAGS) $(LIBS)

clean:
	rm a.out
