QT += core gui

TARGET = chm2qch_gui

TEMPLATE = app

*g++: QMAKE_CXXFLAGS += -std=gnu++11

include(chmlib/chmlib.pri)
include(qt4/qcommandlineparser.pri)

DEFINES += USE_GUI

SOURCES += main.cpp \
    chmfile.cpp \
    hhparser.cpp \
    helper_entitydecoder.cpp \
    converter.cpp \
    dialog.cpp

HEADERS += \
    chmfile.h \
    hhparser.h \
    helper_entitydecoder.h \
    converter.h \
    dialog.h

FORMS += \
    dialog.ui

RESOURCES += \
    resources.qrc
