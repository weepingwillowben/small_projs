TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
#LIBS+= C:/Windows/System32/OpenCL.dll
LIBS += ../../neural_nets/net_prac/exp_intrin.o

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -mavx
QMAKE_CXXFLAGS += -fpermissive

QMAKE_CXXFLAGS_RELEASE += -ffast-math
QMAKE_CXXFLAGS_RELEASE += -mtune=native
QMAKE_CXXFLAGS_RELEASE += -march=native

INCLUDEPATH += "../../headerlib"
INCLUDEPATH += "C:\boost_1_60_0"

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    net_algorithms.hpp \
    layer.h \
    neural_net.h \
    globals.h \
    optimized_procs.h \
    cl_layer.h \
	cl_net.h

DISTFILES += \
    compute.cl \
    oldcode.txt

