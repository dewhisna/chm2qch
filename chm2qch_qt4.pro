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
    helper_entitydecoder.cpp \
    converter.cpp \
    ebook_chm_encoding.cpp \
    qtdirinfo.cpp

HEADERS += \
    chmfile.h \
    hhparser.h \
    helper_entitydecoder.h \
    converter.h \
    ebook_chm_encoding.h \
    qtdirinfo.h
