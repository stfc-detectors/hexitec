#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T10:52:42
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Prevent MSVC 2013 complaining "Conversion from string literal loses const qualifier"
QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
# See: http://stackoverflow.com/a/28625430/2903608

TARGET = DetectorLib
TEMPLATE = lib
CONFIG +=staticlib

DEFINES += DETECTORLIB_LIBRARY DETECTORLIB_EXPORTS

SOURCES += windowsevent.cpp \
    inifile.cpp \
    detectorfactory.cpp \
    detectormonitor.cpp \
    gigedetector.cpp \
    detectorfilename.cpp \
    monitordata.cpp \
    imageacquirer.cpp \
    filewriter.cpp \
    detectorexception.cpp

HEADERS += windowsevent.h \
    inifile.h \
    detectorfactory.h \
    detectormonitor.h \
    gigedetector.h \
    detectorfilename.h \
    monitordata.h \
    imageacquirer.h \
    filewriter.h \
    detectorexception.h

LIBS += GigE.lib Kernel32.lib
INCLUDEPATH += ../DetectorLib

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
