TARGET = yabs
INCLUDEPATH += include
CONFIG-= qt
LIBS+=-lssh2 -lyaml
QMAKE_CXXFLAGS += -std=c++11

QMAKE_CLEAN += yabs.ybf yabs

win32: {
	LIBS+= -lregex
}

g++ {
	QMAKE_CXX = g++
	QMAKE_CC = gcc
}

clang {
	QMAKE_CXX = clang++
	QMAKE_CC = clang
}

HEADERS += include/env.h include/gen.h include/interface.h include/parser.h include/platdef.h
SOURCES += src/env.cpp src/gen.cpp src/interface.cpp src/parser.cpp src/yabs.cpp
