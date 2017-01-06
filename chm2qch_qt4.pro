QT       += core
QT       -= gui

TARGET = chm2qch
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

*g++: QMAKE_CXXFLAGS += -std=gnu++11

include(chmlib/chmlib.pri)
include(qt4/qcommandlineparser.pri)

SOURCES += main.cpp \
    chmfile.cpp \
    hhparser.cpp \
    helper_entitydecoder.cpp

HEADERS += \
    chmfile.h \
    hhparser.h \
    helper_entitydecoder.h
