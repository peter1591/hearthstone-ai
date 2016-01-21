include ../../Makefile.inc

OBJECTS = board.o deck-database.o move.o

OUTPUT_LIB = libGameEngine.a

CFLAGS = $(CFLAGS_PROJECT) -I../include

.PHONY: all
all: build

.PHONY: build
build: $(OUTPUT_LIB)

clean:
	rm -rf $(OBJECTS) $(OUTPUT_LIB)

$(OUTPUT_LIB): $(OBJECTS)
	$(AR) rcs $(OUTPUT_LIB) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $<

