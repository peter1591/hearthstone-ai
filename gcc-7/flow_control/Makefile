CXX=g++-7
CFLAGS=-std=c++17
CFLAGS += -Wall -Wextra -Werror \
					-Wno-implicit-fallthrough \
					-Wno-unused-parameter

#CXX=clang-5.0
#CFLAGS=-std=c++1z

TOP_SOURCE=../../

CFLAGS+=-O2
CFLAGS+=-I$(TOP_SOURCE)include

SRCS=${TOP_SOURCE}contrib/lib_json/json_value.cpp \
		 ${TOP_SOURCE}contrib/lib_json/json_reader.cpp \
		 ${TOP_SOURCE}contrib/lib_json/json_writer.cpp \
		 ${TOP_SOURCE}src/flow_control_test/CardDispatcher.cpp \
		 ${TOP_SOURCE}src/flow_control_test/main.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test2.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test3.cpp \
		 ${TOP_SOURCE}src/flow_control_test/test4.cpp

OBJS=$(SRCS:.cpp=.o)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	cp ${TOP_SOURCE}include/Cards/cards.json .
	$(CXX) $(CFLAGS) $(OBJS) $(LDFLAGS)

clean:
	rm $(OBJS)
