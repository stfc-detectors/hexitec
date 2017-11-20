#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T12:01:59
#
#-------------------------------------------------


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT +=serialport

TARGET = SerialPortLib
CONFIG +=staticlib

TEMPLATE = lib

DEFINES += SERIALPORTLIB_LIBRARY

SOURCES += \
    serialport.cpp

#DEFINES += QEXTSERIALPORT_USING_SHARED

HEADERS += \
    serialport.h

#INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10240.0\ucrt"
