
CXX=clang++-3.8

CFLAGS += -DUSE_BOOST

CFLAGS += -I../include \
          -I../boost/boost_1_61_0 \
          -std=c++14 -stdlib=libc++

SOURCES=../vs_projects/flow_controller/main.cpp \
	../vs_projects/flow_controller/test1.cpp \
	../vs_projects/flow_controller/test2.cpp \
	../vs_projects/flow_controller/test3.cpp \
	../vs_projects/flow_controller/test4.cpp

all:
	$(CXX) $(CFLAGS) $(SOURCES)
