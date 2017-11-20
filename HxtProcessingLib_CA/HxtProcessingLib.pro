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
    src/Timer.cpp \
    src/LogConfig.cpp \
    src/Log.cpp \
    src/HxtSubPixelFrame.cpp \
    src/HxtRawDataProcessor.cpp \
    src/HxtPixelThreshold.cpp \
    src/HxtPixelArray.cpp \
    src/HxtFrameInterpolateCorrector.cpp \
    src/HxtFrameInducedNoiseCorrector.cpp \
    src/HxtFrameIncompleteDataCorrector.cpp \
    src/HxtFrameDoublePixelsCorrector.cpp \
    src/HxtFrameChargeSharingSubPixelCorrector.cpp \
    src/HxtFrameChargeSharingDiscCorrector.cpp \
    src/HxtFrameCalibrationCorrector.cpp \
    src/HxtFrame.cpp \
    src/HxtDecodedFrame.cpp \
    src/Histogram.cpp \
    src/DateStamp.cpp \
    src/HxtProcessingTester.cpp \
    src/HxtFrameMomentumCorrector.cpp

HEADERS += \
    include/Timer.h \
    include/LogConfig.h \
    include/Log.h \
    include/HxtSubPixelFrame.h \
    include/HxtRawDataTypes.h \
    include/HxtRawDataProcessor.h \
    include/HxtPixelThreshold.h \
    include/HxtPixelArray.h \
    include/HxtPixel.h \
    include/HxtOutputFormat.h \
    include/HxtLogger.h \
    include/HxtFrameInterpolateCorrector.h \
    include/HxtFrameInducedNoiseCorrector.h \
    include/HxtFrameIncompleteDataCorrector.h \
    include/HxtFrameDoublePixelsCorrector.h \
    include/HxtFrameCorrector.h \
    include/HxtFrameChargeSharingSubPixelCorrector.h \
    include/HxtFrameChargeSharingDiscCorrector.h \
    include/HxtFrameCalibrationCorrector.h \
    include/HxtFrame.h \
    include/HxtDecodedFrame.h \
    include/Histogram.h \
    include/DateStamp.h \
    include/HxtProcessingTester.h \
    include/HxtFrameMomentumCorrector.h

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
