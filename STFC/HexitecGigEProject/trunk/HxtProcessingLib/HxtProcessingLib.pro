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
    pixelprocessor.cpp \
    pixelprocessornextframe.cpp \
    imageitem.cpp \
    bufferitem.cpp \
    processingdefinition.cpp \
    imageprocessor.cpp \
    frameprocessor.cpp \
    framere_orderprocessor.cpp \
    generalframeprocessor.cpp \
    hxtgenerator.cpp \
    generalhxtgenerator.cpp \
    hxtitem.cpp \
    generalpixelprocessor.cpp \
    hxtchargedsharinggenerator.cpp \
    hxtsumgenerator.cpp \
    hxtchargedsharingsumgenerator.cpp


HEADERS += \
    pixelprocessor.h \
    pixelprocessornextframe.h \
    imageitem.h \
    bufferitem.h \
    processingdefinition.h \
    imageprocessor.h \
    frameprocessor.h \
    framere_orderprocessor.h \
    generalframeprocessor.h \
    hxtgenerator.h \
    generalhxtgenerator.h \
    hxtitem.h \
    generalpixelprocessor.h \
    hxtchargedsharinggenerator.h \
    hxtsumgenerator.h \
    hxtchargedsharingsumgenerator.h


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
