#-------------------------------------------------
#
# Project created by QtCreator 2016-04-09T12:02:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = path
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -march=native -mtune=native

win32{
    INCLUDEPATH += "../../../../Visual Studio 2013/"
}
unix{
    INCLUDEPATH += "../../headerlib/"
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    board.h

FORMS    += mainwindow.ui
