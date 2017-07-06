CXX=g++-7

CFLAGS=-std=c++17 -O2
CFLAGS+=-I../include

SRCS=../contrib/lib_json/json_value.cpp \
		 ../contrib/lib_json/json_reader.cpp \
		 ../contrib/lib_json/json_writer.cpp \
		 ../src/flow_control_test/CardDispatcher.cpp \
		 ../src/flow_control_test/main.cpp \
		 ../src/flow_control_test/test2.cpp \
		 ../src/flow_control_test/test3.cpp \
		 ../src/flow_control_test/test4.cpp

OBJS=$(SRCS:.cpp=.o)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

all: $(OBJS)
	cp ../include/Cards/cards.json .
	$(CXX) $(CFLAGS) $(OBJS) $(LDFLAGS)

clean:
	rm $(OBJS)
