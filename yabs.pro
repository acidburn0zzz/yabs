TARGET = yabs
INCLUDEPATH += include
CONFIG-= qt
LIBS+=-lssh -lyaml

QMAKE_CLEAN += yabs.ybf yabs

g++ {
	QMAKE_CXX = g++
	QMAKE_CC = gcc
}

clang {
	QMAKE_CXX = clang++
	QMAKE_CC = clang
}

HEADERS += include/env.h include/gen.h include/interface.h include/parser.h
SOURCES += src/env.cpp src/gen.cpp src/interface.cpp src/parser.cpp src/yabs.cpp
