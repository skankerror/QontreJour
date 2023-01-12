TEMPLATE = lib
LANGUAGE = C++
TARGET   = QDmxESPNet

QT      += network
QT      -= gui

CONFIG      += plugin

win32:QMAKE_LFLAGS += -shared

CONFIG(debug, debug|release){
    unix: target.path = /Users/nicolas/Programmation/plugin
    win32:  target.path = E:/Prog/plugin
    INSTALLS   += target
}

CONFIG += network_interface

include(../../interface/interface.pri)

HEADERS += QDmxESPNetPlugin.h \
    QDmxESPNetControler.h \
    QDmxESPNetPacketizer.h

SOURCES += \
    QDmxESPNetControler.cpp \
    QDmxESPNetPacketizer.cpp
