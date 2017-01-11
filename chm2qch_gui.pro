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
