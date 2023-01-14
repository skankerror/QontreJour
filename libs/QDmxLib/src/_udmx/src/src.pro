#-------------------------------------------------
#
# Project created by QtCreator 2018-01-01T18:07:16
#
#-------------------------------------------------

QT       -= gui

TARGET = QDmxUDmx
TEMPLATE = lib
LANGUAGE = C++
CONFIG += plugin

win32:QMAKE_LFLAGS += -shared

CONFIG(debug, debug|release){
    target.path = /Users/nicolas/Programmation/plugin
    INSTALLS   += target
}

include(../../interface/interface.pri)

LIBS += -L/opt/local/lib -lusb
INCLUDEPATH += /opt/local/include/

SOURCES += \
        QDmxUDmxPlugin.cpp \
    QDmxUDmxDevice.cpp

HEADERS += \
        QDmxUDmxPlugin.h \
    QDmxUDmxDevice.h
