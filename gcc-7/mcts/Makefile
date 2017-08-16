CXX=g++-7
CFLAGS=-std=c++17
CFLAGS += -Wall -Wextra -Wpedantic \
					-Wno-implicit-fallthrough \
					-Wno-unused-parameter \
					-Werror
CFLAGS_OWN_SRC += -Weffc++

#CXX=clang-5.0
#CFLAGS=-std=c++1z

TOP_SOURCE=../../

CFLAGS+=-I$(TOP_SOURCE)include
CFLAGS+=-ggdb
LDFLAGS=-lpthread

# release build
CFLAGS+=-O3 -march=native
CFLAGS+=-DNDEBUG
LDFLAGS+=-O3

ifneq ($(PROFILING), 2)
# Preserve the symbols for PROFILING=2
CFLAGS+=-flto -fuse-linker-plugin
LDFLAGS+=-flto -fuse-linker-plugin
endif

# profiling
ifeq ($(PROFILING), 1)
	CFLAGS+=-fprofile-generate
	LDFLAGS+=-fprofile-generate
else
	ifeq ($(PROFILING), 2)
		# Use gperftools
	else
		CFLAGS += -fprofile-use
		LDFLAGS+= -fprofile-use
	endif
endif

THIRD_PARTY_SRCS=${TOP_SOURCE}contrib/lib_json/json_value.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_reader.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_writer.cpp
THIRD_PARTY_OBJS=$(THIRD_PARTY_SRCS:.cpp=.o)

SRCS=${TOP_SOURCE}src/MCTS/CardDispatcher.cpp \
     ${TOP_SOURCE}src/MCTS/TestStateBuilder.cpp \
     ${TOP_SOURCE}src/MCTS/test.cpp
OBJS=$(SRCS:.cpp=.o)

CARDS_JSON="cards.json"
CARDS_JSON_SRC=${TOP_SOURCE}include/Cards/cards.json

EXE=mcts_test

.PHONY:
all: $(EXE) $(CARDS_JSON)
	@echo "Done."

$(CARDS_JSON): ${CARDS_JSON_SRC}
	cp ${CARDS_JSON_SRC} ${CARDS_JSON}

$(THIRD_PARTY_OBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJS): %.o: %.cpp
	$(CXX) $(CFLAGS) $(CFLAGS_OWN_SRC) -c $< -o $@

.PHONY:
$(EXE): $(THIRD_PARTY_OBJS) $(OBJS)
	$(CXX) $(THIRD_PARTY_OBJS) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -f ${CARDS_JSON} ${THIRD_PARTY_OBJS} $(OBJS) $(EXE)

cpu:
	rm -f ./prof.result
	LD_PRELOAD=/usr/lib/libtcmalloc_and_profiler.so.4 CPUPROFILE=./prof.result ./$(EXE)
	google-pprof ./$(EXE) ./prof.result

tcmalloc:
	LD_PRELOAD=/usr/lib/libtcmalloc.so.4 ./$(EXE)
