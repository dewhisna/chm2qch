QT       += core
QT       -= gui

TARGET = chm2qch
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++11

TEMPLATE = app

include(chmlib/chmlib.pri)

SOURCES += main.cpp \
    chmfile.cpp \
    hhparser.cpp \
    helper_entitydecoder.cpp \
    converter.cpp \
    ebook_chm_encoding.cpp

HEADERS += \
    chmfile.h \
    hhparser.h \
    helper_entitydecoder.h \
    converter.h \
    ebook_chm_encoding.h
