#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T10:52:42
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DetectorLib
TEMPLATE = lib
CONFIG +=staticlib

DEFINES += DETECTORLIB_LIBRARY HEXITEC_EXPORTS

SOURCES += windowsevent.cpp \
    inifile.cpp \
    aspectdetector.cpp

HEADERS +=DetectorLib_global.h \
    windowsevent.h \
    inifile.h \
    aspectdetector.h

LIBS += Hexitec.lib Kernel32.lib
CONFIG(release, debug|release): LIBS += ../TemperatureHumidityLib/release/TemperatureHumidityLib.lib
CONFIG(debug, debug|release): LIBS += ../TemperatureHumidityLib/debug/TemperatureHumidityLib.lib
INCLUDEPATH += ../DetectorLib
INCLUDEPATH += ../TemperatureHumidityLib

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
