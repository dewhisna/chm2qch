QT += core gui widgets

TARGET = chm2qch_gui
CONFIG += c++11

TEMPLATE = app

include(chmlib/chmlib.pri)

DEFINES += USE_GUI

SOURCES += main.cpp \
    chmfile.cpp \
    hhparser.cpp \
    helper_entitydecoder.cpp \
    converter.cpp \
    dialog.cpp \
    ebook_chm_encoding.cpp \
    qtdirinfo.cpp

HEADERS += \
    chmfile.h \
    hhparser.h \
    helper_entitydecoder.h \
    converter.h \
    dialog.h \
    ebook_chm_encoding.h \
    qtdirinfo.h

FORMS += \
    dialog.ui

RESOURCES += \
    resources.qrc

#Common controls manifest (GCC doesn't add it automatically)
win32-g++: RC_FILE = chm2qch.rc
