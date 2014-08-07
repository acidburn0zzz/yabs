CC	= gcc
CXX 	= g++
DEFINES =
CFLAGS 	= -pipe -march=x86-64 -mtune=generic -O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -Wall -W $(DEFINES)
CXXFLAGS= -pipe -std=c++11 -march=x86-64 -mtune=generic -O2 -pipe -fstack-protector --param=ssp-buffer-size=4 -Wall -W $(DEFINES)
LINK 	= g++
LFLAGS 	= -Wl
LIBS 	= -lssh2 -lyaml
INCPATH = -I/usr/include -Iinclude
DEL	= rm -f
DEL_R	= rm -r
SRC 	= src/env.cpp \
	  src/gen.cpp \
	  src/interface.cpp \
	  src/parser.cpp \
	  src/yabs.cpp
OBJ 	= env.o \
	  gen.o \
	  interface.o \
	  parser.o \
	  yabs.o
TRGT 	= yabs

first: all

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

all: $(TRGT)

$(TRGT): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TRGT) $(OBJ) $(LIBS)

env.o: src/env.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o env.o src/env.cpp

gen.o: src/gen.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gen.o src/gen.cpp

interface.o: src/interface.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o interface.o src/interface.cpp

parser.o: src/parser.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o parser.o src/parser.cpp

yabs.o: src/yabs.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o yabs.o src/yabs.cpp

clean:
	$(DEL) $(OBJ)
	$(DEL) yabs.ybf yabs
	$(DEL) *~ core *.core
