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

DEFINES += DETECTORLIB_LIBRARY HEXITEC_EXPORTS

SOURCES += windowsevent.cpp \
    inifile.cpp \
#    aspectdetector.cpp \
    detectorfactory.cpp \
    detectormonitor.cpp \
    gigedetector.cpp \
    detectorfilename.cpp \
    monitordata.cpp \
    imageacquirer.cpp

HEADERS +=DetectorLib_global.h \
    windowsevent.h \
    inifile.h \
#    aspectdetector.h \
    detectorfactory.h \
    detectormonitor.h \
    gigedetector.h \
    detectorfilename.h \
    monitordata.h \
    imageacquirer.h

LIBS += Hexitec.lib GigE.lib Kernel32.lib
LIBS += GigE.lib Kernel32.lib
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
