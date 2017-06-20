#-------------------------------------------------
#
# Project created by QtCreator 2013-12-05T11:22:33
#
#-------------------------------------------------

#QT       -= core gui

TARGET = HxtProcessingLib
TEMPLATE = lib
CONFIG += staticlib

DEFINES += OS_WIN=1

SOURCES += \
    frameProcessor.cpp \
    pixelProcessor.cpp


HEADERS += \
    frameProcessor.h \
    pixelProcessor.h


INCLUDEPATH += include
INCLUDEPATH += src

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
