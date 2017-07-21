QT += core gui widgets
TEMPLATE = app
#CONFIG += console c++11
#CONFIG -= app_bundle
QMAKE_LFLAGS += /STACK:1120000000

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Test

SOURCES += main.cpp \
    hxtgeneratortester.cpp \
    mainwindow.cpp \
    processingForm.cpp \
    processingbuffergenerator.cpp

INCLUDEPATH += ../HxtProcessingLib

LIBS += ../HxtProcessingLib/release/HxtProcessingLib.lib

HEADERS += \
    hxtgeneratortester.h \
    mainwindow.h \
    processingForm.h \
    processingbuffergenerator.h

FORMS += \
    mainwindow.ui \
    processingForm.ui

RESOURCES += \
    Test.qrc