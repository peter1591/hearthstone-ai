include ../../Makefile.inc

CFLAGS += $(CFLAGS_PROJECT) -I../../include
LIBS = ../../lib/libsimulation.a

all:
	g++ $(CFLAGS) test.cpp $(LIBS)

i:
	g++ $(CFLAGS) -DINTERACTIVE test.cpp $(LIBS)
