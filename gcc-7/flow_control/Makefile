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

CFLAGS+=-O2
CFLAGS+=-I$(TOP_SOURCE)include

THIRD_PARTY_SRCS=${TOP_SOURCE}contrib/lib_json/json_value.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_reader.cpp \
								 ${TOP_SOURCE}contrib/lib_json/json_writer.cpp
THIRD_PARTY_OBJS=$(THIRD_PARTY_SRCS:.cpp=.o)

SRCS=${TOP_SOURCE}src/flow_control_test/CardDispatcher.cpp \
		 ${TOP_SOURCE}src/flow_control_test/main.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test2.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test3.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test4.cpp
OBJS=$(SRCS:.cpp=.o)

CARDS_JSON="cards.json"
CARDS_JSON_SRC=${TOP_SOURCE}include/Cards/cards.json

EXE=flow_control_test

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
