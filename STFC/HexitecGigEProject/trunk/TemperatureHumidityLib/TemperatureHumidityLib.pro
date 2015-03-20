#-------------------------------------------------
#
# Project created by QtCreator 2013-04-23T20:19:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TemperatureHumidityLib
TEMPLATE = lib
CONFIG += staticlib

DEFINES += TEMPERATUREHUMIDITYLIB_LIBRARY

SOURCES += temperature.cpp \
    sht21temperature.cpp \
    sht21controller.cpp \
    sht21humidity.cpp \
    humidity.cpp \
    temperaturecontroller.cpp \
    west6100plustemperaturecontroller.cpp

HEADERS  += temperature.h \
    sht21temperature.h \
    sht21controller.h \
    sht21humidity.h \
    humidity.h \
    temperaturecontroller.h \
    temperaturecontroller.h \
    west6100plustemperaturecontroller.h

LIBS += $$quote(-LC:/Program Files/fieldtalk/lib/win/x64/release/) -llibmbusmaster
LIBS += iowkit.lib

INCLUDEPATH += "C:/Program Files/fieldtalk/include/"
