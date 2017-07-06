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
    pixelProcessor.cpp \
    imageitem.cpp \
    bufferitem.cpp \
    processingdefinition.cpp \
    imageprocessor.cpp \
    frameprocessor.cpp \
    framere_orderprocessor.cpp \
    generalframeprocessor.cpp \
    hxtgenerator.cpp \
    generalhxtgenerator.cpp \
    hxttotalspectrumgenerator.cpp


HEADERS += \
    pixelProcessor.h \
    imageitem.h \
    bufferitem.h \
    processingdefinition.h \
    imageprocessor.h \
    frameprocessor.h \
    framere_orderprocessor.h \
    generalframeprocessor.h \
    hxtgenerator.h \
    generalhxtgenerator.h \
    hxttotalspectrumgenerator.h


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
