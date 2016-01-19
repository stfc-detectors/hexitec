#-------------------------------------------------
#
# Project created by QtCreator 2014-03-17T16:34:38
#
#-------------------------------------------------

QT       -= gui

# Prevent MSVC 2013 complaining "Conversion from string literal loses const qualifier"
QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
# See: http://stackoverflow.com/a/28625430/2903608

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
