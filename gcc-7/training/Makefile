#CXX=g++-7
CXX=/usr/local/gcc-7.1/bin/g++-7.1

CFLAGS=-std=c++17

TOP_SOURCE=../../

CFLAGS+=-I${TOP_SOURCE}include \
        -I${TOP_SOURCE}contrib/tiny-dnn
CFLAGS+=-g
LDFLAGS+=-lpthread

# release build
CFLAGS+=-O3 -march=native
#CFLAGS+=-DNDEBUG
LDFLAGS+=-O3

THIRD_PARTY_SRCS=${TOP_SOURCE}contrib/lib_json/json_value.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_reader.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_writer.cpp
THIRD_PARTY_OBJS=$(THIRD_PARTY_SRCS:.cpp=.o)

SRCS=${TOP_SOURCE}src/Train/Source.cpp
OBJS=$(SRCS:.cpp=.o)

EXE=train

.PHONY:
all: $(EXE)
	@echo "Done."

$(THIRD_PARTY_OBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

.PHONY:
$(EXE): $(THIRD_PARTY_OBJS) $(OBJS)
	$(CXX) $(THIRD_PARTY_OBJS) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -f ${THIRD_PARTY_OBJS} $(OBJS) $(EXE)

run:
	LD_LIBRARY_PATH=/usr/local/gcc-7.1/lib64 ./$(EXE)

tcmalloc:
	LD_LIBRARY_PATH=/usr/local/gcc-7.1/lib64 LD_PRELOAD=/usr/lib/libtcmalloc.so.4 ./$(EXE)
