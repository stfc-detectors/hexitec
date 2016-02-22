#-------------------------------------------------
#
# Project created by QtCreator 2011-11-24T10:35:18
#
#-------------------------------------------------
QT       += core gui script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Prevent MSVC 2013 complaining "Conversion from string literal loses const qualifier"
QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings
# See: http://stackoverflow.com/a/28625430/2903608

TARGET = HexitecGigE
TEMPLATE = app

SOURCES += main.cpp \
    voxel.cpp \
    thumbviewer.cpp \
    slice.cpp \
    scriptingwidget.cpp \
    sarray.cpp \
    renderarea.cpp \
    plotter.cpp \
    plotsettings.cpp \
    motorlimits.cpp \
    motor.cpp \
    mainwindow.cpp \
    hxtframe.cpp \
    hxtfilereader.cpp \
    gridsizequery.cpp \
    dummymotor.cpp \
    motioncontrolform.cpp \
    curve.cpp \
    colormap.cpp \
    chargesharing.cpp \
    axissettings.cpp \
    treeitem.cpp \
    colormapeditor.cpp \
    motormodel.cpp \
    motordelegate.cpp \
    dataacquisitionfactory.cpp \
    motorfactory.cpp \
    galilmotorcontroller.cpp \
    galilmotor.cpp \
    matlab.cpp \
    applicationoutput.cpp \
    hexitecsofttrigger.cpp \
    reservable.cpp \
    scriptrunner.cpp \
    objectreserver.cpp \
    reservation.cpp \
    motorstatus.cpp \
    volume.cpp \
    datamodel.cpp \
    mainviewer.cpp \
    workspace.cpp \
    getvaluedialog.cpp \
    matlabvariable.cpp \
    eigencomponent.cpp \
    offsetsdialog.cpp \
    datacollectiondialog.cpp \
    dpwarningdialog.cpp \
    vboutofrangedialog.cpp \
    detectorcontrolform.cpp \
    dataacquisitionform.cpp \
    dataacquisition.cpp \
    voltageSourceFactory.cpp \
    dataacquisitionmodel.cpp \
    detectorfilename.cpp \
    dataacquisitiondefinition.cpp \
    hxtprocessing.cpp \
    processingwindow.cpp \
    dataacquisitionstatus.cpp \
    newportxpsmotor.cpp \
    setaxesdialog.cpp \
    displaylegendsdialog.cpp \
    parameters.cpp \
    hv.cpp \
    badinifiledialog.cpp \  
    progressform.cpp

HEADERS += mainwindow.h \
    voxel.h \
    slice.h \
    scriptingwidget.h \
    sarray.h \
    renderarea.h \
    plotter.h \
    plotsettings.h \
    motorlimits.h \
    motor.h \
    hxtframe.h \
    hxtfilereader.h \
    gridsizequery.h \
    dummymotor.h \
    motioncontrolform.h \
    curve.h \
    colormap.h \
    chargesharing.h \
    axissettings.h \
    thumbviewer.h \
    treeitem.h \
    colormapeditor.h \
    motormodel.h \
    motordelegate.h \
    dataacquisitionfactory.h \
    motorfactory.h \
    galilmotorcontroller.h \
    galilmotor.h \
    matlab.h \
    applicationoutput.h \
#    hardtrigger.h \
    hexitecsofttrigger.h \
    reservable.h \
    scriptrunner.h \
    objectreserver.h \
    reservation.h \
    motorstatus.h \
    volume.h \
    datamodel.h \
    mainviewer.h \
    workspace.h \
    getvaluedialog.h \
    matlabvariable.h \
    eigencomponent.h \
    offsetsdialog.h \
    datacollectiondialog.h \
    dpwarningdialog.h \
    vboutofrangedialog.h \
    detectorcontrolform.h \
    dataacquisitionform.h \
    dataacquisition.h \
    voltageSourceFactory.h \
    dataacquisitionmodel.h \
    detectorfilename.h \
    dataacquisitiondefinition.h \
    hxtprocessing.h \
    processingwindow.h \
    dataacquisitionstatus.h \
    newportxpsmotor.h \
    setaxesdialog.h \
    displaylegendsdialog.h \
    parameters.h \
    hv.h \
    badinifiledialog.h \
    progressform.h

FORMS += \
    gridsizequery.ui \
    chargesharing.ui \
    scriptingWidget.ui \
    colormapeditor.ui \
    getvaluedialog.ui \
    offsetsdialog.ui \
    datacollectiondialog.ui \
    dpwarningdialog.ui \
    vboutofrangedialog.ui \
    motioncontrolform.ui \
    detectorcontrolform.ui \
    dataacquisitionform.ui \
    processingwindow.ui \
    setaxesdialog.ui \
    displaylegendsdialog.ui \
    badinifiledialog.ui \
    progressform.ui

RESOURCES += \
    HexitecGigE.qrc

# The HexitecGigE_SYSTEM environment variable determines which configuration is performed next.
CONFIG += $$(HEXITECGIGE_SYSTEM)

DSoFt64 {
  # DSoFt 64 bit configuration
  DEFINES += NONI
  DEFINES += OS_WIN
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2012b/extern/lib/win64/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  LIBS += ../DetectorLib/GigE.lib /DELAYLOAD:GigE.dll
  CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(release, debug|release): LIBS += ../DetectorLib/release/DetectorLib.lib ../SerialPortLib/release/SerialPortLib.lib
  CONFIG(release, debug|release): LIBS += ../HxtProcessingLib/release/HxtProcessingLib.lib ../XpsLib/release/XpsLib.lib -L../SerialPortLib/qextserialport/release -lQt5ExtSerialPort1 /DELAYLOAD:XpsLib.dll
  CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(debug, debug|release): LIBS += ../DetectorLib/debug/DetectorLib.lib ../SerialPortLib/debug/SerialPortLib.lib
  CONFIG(debug, debug|release): LIBS += ../HxtProcessingLib/debug/HxtProcessingLib.lib ../XpsLib/debug/XpsLib.lib -L../SerialPortLib/qextserialport/debug -lQt5ExtSerialPortd1 /DELAYLOAD:XpsLib.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2012b/extern/include/"
  INCLUDEPATH += "C:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64"
  INCLUDEPATH += ../Eigen/3_1_2/Eigen
  INCLUDEPATH += ../DetectorLib
  INCLUDEPATH  += ../HxtProcessingLib/include
  INCLUDEPATH += ../XpsLib
  INCLUDEPATH += ../SerialPortLib
  INCLUDEPATH += ../SerialPortLib/qextserialport/src
  DEFINES += QEXTSERIALPORT_USING_SHARED
} else:DSoFt32 {
  # DSoFt 32 bit configuration
  DEFINES += NONI
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2011b/extern/lib/win32/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  LIBS += $$quote(-LC:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/lib32/msvc/) -lNIDAQmx
  LIBS += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/lib32/msvc/ni4882.obj"
  CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2011b/extern/include/"
  INCLUDEPATH += "C:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86"
  INCLUDEPATH += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/include/"
  INCLUDEPATH += "C:/shk/TEDDI/Eigen/3_1_2/Eigen/"
} else:DSoFt32_NI {
  DEFINES += NI
  # DSoFt 32 bit configuration
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2012b/extern/lib/win32/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  LIBS += $$quote(-LC:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/lib32/msvc/) -lNIDAQmx
  LIBS += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/lib32/msvc/ni4882.obj"
  CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2011b/extern/include/"
  INCLUDEPATH += "C:/Program Files/Galil/GalilTools/LibGalil-2.0.0.436-vs2008-x86"
  INCLUDEPATH += "C:/Program Files/National Instruments/Shared/ExternalCompilerSupport/C/include/"
  INCLUDEPATH += "C:/shk/TEDDI/Eigen/3_1_2/Eigen/"
} else:aSpect32 {
  DEFINES += NI
  # aSpect PC configuration
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2011b/extern/lib/win32/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  LIBS += $$quote(-LC:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/Lib32/msvc/) -lNIDAQmx
  LIBS += "C:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/Lib32/MSVC/ni4882.obj"
  CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.464-vs2010-x86/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.464-vs2010-x86/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2011b/extern/include/"
  INCLUDEPATH += "C:/Program Files/Galil/GalilTools/LibGalil-2.0.1.464-vs2010-x86"
  INCLUDEPATH += "C:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/include/"
  INCLUDEPATH += "C:/DSoFt/Eigen/3_1_2/Eigen/"
} else:aSpect64 {
  # aSpect PC configuration
  DEFINES += NI
  DEFINES += OS_WIN
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2012b/extern/lib/win64/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  LIBS += $$quote(-LC:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/Lib64/msvc/) -lNIDAQmx
  LIBS += "C:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/Lib64/MSVC/ni4882.obj"
  LIBS += ../DetectorLib/GigE.lib /DELAYLOAD:GigE.dll
  LIBS += $$quote(-LC:/Program Files/fieldtalk/lib/win/x64/release/) -llibmbusmaster
  LIBS += ../TemperatureHumidityLib/iowkit.lib /DELAYLOAD:iowkit.dll
  CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(release, debug|release): LIBS += ../DetectorLib/release/DetectorLib.lib ../TemperatureHumidityLib/release/TemperatureHumidityLib.lib ../SerialPortLib/release/SerialPortLib.lib
  CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  CONFIG(debug, debug|release): LIBS += ../DetectorLib/debug/DetectorLib.lib ../TemperatureHumidityLib/debug/TemperatureHumidityLib.lib ../SerialPortLib/debug/SerialPortLib.lib
  INCLUDEPATH += "C:/Program Files/MATLAB/R2012b/extern/include/"
  INCLUDEPATH += "C:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2010-x64"
  INCLUDEPATH += "C:/Program Files (x86)/National Instruments/Shared/ExternalCompilerSupport/C/include/"
  INCLUDEPATH += "../Eigen/3_1_2/Eigen/"
  INCLUDEPATH += ../DetectorLib
  INCLUDEPATH += ../TemperatureHumidityLib
  LIBS += ../HxtProcessingLib/release/HxtProcessingLib.lib
  INCLUDEPATH  += ../HxtProcessingLib/include
  LIBS += ../XpsLib/release/XpsLib.lib /DELAYLOAD:XpsLib.dll
  INCLUDEPATH += ../XpsLib
  INCLUDEPATH += ../SerialPortLib
  INCLUDEPATH += ../SerialPortLib/qextserialport/src
  LIBS += -L../SerialPortLib/qextserialport/release -lQt5ExtSerialPort1
  DEFINES += QEXTSERIALPORT_USING_SHARED

} else:SDMJ_laptop {
  # SDMJ Laptop configuration
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2011a/extern/lib/win32/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2011a/extern/include/"
} else:SDMJ_work {
  # SDMJ work configuration
  LIBS += $$quote(-LC:/Program Files/MATLAB/R2010a/extern/lib/win64/microsoft/) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
  win32:CONFIG(release, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2008-x64/release/) -lGalil2 /DELAYLOAD:Galil2.dll
  else:win32:CONFIG(debug, debug|release): LIBS += $$quote(-LC:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2008-x64/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
  INCLUDEPATH += "C:/Program Files/MATLAB/R2010a/extern/include/"
  INCLUDEPATH += $$quote(C:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2008-x64)
  DEPENDPATH += $$quote(C:/Program Files/Galil/GalilTools/LibGalil-2.0.1.447-vs2008-x64)
} else:ckd_devel {

#    !exists(torso.cpp) {
#        error("No such file found - Windows environment variable is working then..")
#    }

    # te2aspect14 PC 64 bit configuration
    DEFINES += NONI
    DEFINES += OS_WIN
    LIBS += $$quote(-LC:/Program Files/MATLAB/R2014a/extern/lib/win64/microsoft) -llibeng -llibmx delayimp.lib /DELAYLOAD:libeng.dll /DELAYLOAD:libmx.dll
    LIBS += ../DetectorLib/GigE.lib /DELAYLOAD:GigE.dll

    CONFIG(release, debug|release): LIBS += $$quote(-LC:/QT/Libs/LibGalil-2.0.1.447-vs2010-x64/release) -lGalil2 /DELAYLOAD:Galil2.dll
    CONFIG(release, debug|release): LIBS += ../DetectorLib/release/DetectorLib.lib ../SerialPortLib/release/SerialPortLib.lib
    CONFIG(release, debug|release): LIBS += ../HxtProcessingLib/release/HxtProcessingLib.lib ../XpsLib/release/XpsLib.lib -L../SerialPortLib/qextserialport/release -lQt5ExtSerialPort1 /DELAYLOAD:XpsLib.dll
    CONFIG(debug, debug|release): LIBS += $$quote(-LC:/QT/Libs/LibGalil-2.0.1.447-vs2010-x64/debug/) -lGalil2 /DELAYLOAD:Galil2.dll
    CONFIG(debug, debug|release): LIBS += ../DetectorLib/debug/DetectorLib.lib ../SerialPortLib/debug/SerialPortLib.lib
    CONFIG(debug, debug|release): LIBS += ../HxtProcessingLib/debug/HxtProcessingLib.lib ../XpsLib/debug/XpsLib.lib -L../SerialPortLib/qextserialport/debug -lQt5ExtSerialPortd1 /DELAYLOAD:XpsLib.dll

    INCLUDEPATH += "C:/Program Files/MATLAB/R2014a/extern/include"
    INCLUDEPATH += "C:/QT/libs/LibGalil-2.0.1.447-vs2010-x64"
    INCLUDEPATH += "C:/Qt/Libs/Eigen/3.1.4/Eigen/"
    INCLUDEPATH += ../DetectorLib
    INCLUDEPATH  += ../HxtProcessingLib/include
    INCLUDEPATH += ../XpsLib
    INCLUDEPATH += ../SerialPortLib
    INCLUDEPATH += ../SerialPortLib/qextserialport/src
    DEFINES += QEXTSERIALPORT_USING_SHARED
}



