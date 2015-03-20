#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T16:34:38
#
#-------------------------------------------------

QT       -= gui

TARGET = XpsLib
TEMPLATE = lib
###CONFIG += dll
###CONFIG += staticlib
###DEFINES += -DX_EXPORT=Q_DECL_EXPORT
###CONFIG -= dll
###CONFIG += shared_and_static

DEFINES += XPS_SHARED_LIBRARY

SOURCES += \
    newportxpsmotorcontroller.cpp
###LIBS += -lXPS_Q8_drivers

HEADERS += \
    newportxpsmotorcontroller.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
