TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++11 -fpermissive
QMAKE_CXXFLAGS -= -O2
QMAKE_CXXFLAGS += -O3 -march=native -mtune=native
LIBS+= C:/Windows/System32/OpenCL.dll

INCLUDEPATH += C:/boost_1_60_0

SOURCES += main.cpp \
    mat_mult.cpp


DISTFILES += \
    prac.cl

HEADERS += \
    errorhandling.hpp \
    mat_mult.h \
    shared.h

