#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T12:01:59
#
#-------------------------------------------------


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialPortLib
CONFIG +=staticlib

TEMPLATE = lib

DEFINES += SERIALPORTLIB_LIBRARY

SOURCES += \
    serialport.cpp \

INCLUDEPATH += qextserialport/src
LIBS += -Lqextserialport/release -lQt5ExtSerialPort1
DEFINES += QEXTSERIALPORT_USING_SHARED

HEADERS += \
    serialport.h \
