CXX=g++-7.2
CFLAGS=-std=c++17
CFLAGS_OWN_SRC += -Wall -Wextra -Wpedantic \
									-Wno-implicit-fallthrough \
									-Wno-unused-parameter \
									-Werror -Weffc++

#CXX=clang-5.0
#CFLAGS=-std=c++1z

TOP_SOURCE=../../../../

CFLAGS+=-I$(TOP_SOURCE)engine/include \
				-I$(TOP_SOURCE)judge/include \
				-I$(TOP_SOURCE)agents/include \
				-I$(TOP_SOURCE)third_party/jsoncpp/include \
				-I${TOP_SOURCE}third_party/tiny-dnn
CFLAGS+=-ggdb
LDFLAGS=-lpthread

# release build
CFLAGS+=-O3 -march=native
#CFLAGS+=-DNDEBUG
LDFLAGS+=-O3

# Profiling options:
# 0 -> No linker optimization, so google-pprof can see debug symbols
# 1 -> Generate profile for optimization
# 2 -> Use profile to optimize

LINKER_OPTIMIZATION := 0

# profiling
ifeq ($(PROFILING), 1)
LINKER_OPTIMIZATION := 1
CFLAGS+=-fprofile-generate
LDFLAGS+=-fprofile-generate
endif

ifeq ($(PROFILING), 2)
LINKER_OPTIMIZATION := 1
CFLAGS += -fprofile-use
LDFLAGS+= -fprofile-use
endif

ifeq ($(LINKER_OPTIMIZATION), 1)
CFLAGS+=-flto -fuse-linker-plugin
LDFLAGS+=-flto -fuse-linker-plugin
endif

THIRD_PARTY_SRCS=${TOP_SOURCE}third_party/jsoncpp/src/json_value.cpp \
								 ${TOP_SOURCE}third_party/jsoncpp/src/json_reader.cpp \
								 ${TOP_SOURCE}third_party/jsoncpp/src/json_writer.cpp \
								 ${TOP_SOURCE}agents/src/neural_net/NeuralNetwork.cpp
THIRD_PARTY_OBJS=$(THIRD_PARTY_SRCS:.cpp=.o)

SRCS=${TOP_SOURCE}agents/test/CardDispatcher.cpp \
     ${TOP_SOURCE}agents/test/TestStateBuilder.cpp \
     ${TOP_SOURCE}agents/train/src/GenerateTrainData.cpp
OBJS=$(SRCS:.cpp=.o)

CARDS_JSON="cards.json"
CARDS_JSON_SRC=${TOP_SOURCE}include/Cards/cards.json

EXE=generate_train_data

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
	LD_LIBRARY_PATH=/usr/local/gcc-7.1/lib64 LD_PRELOAD=/usr/lib/libtcmalloc.so.4 ./$(EXE)
