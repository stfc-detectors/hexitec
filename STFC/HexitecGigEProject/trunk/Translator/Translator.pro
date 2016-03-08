TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    filereader.cpp

#LIBS += $$quote(C:/Program Files/HDF_Group/HDF5/1.8.16/lib/libhdf5_cpp.lib)
LIBS += $$quote(C:/Program Files/HDF_Group/HDF5/HDF5-1.8.16-win64/lib/libszip.lib)
LIBS += $$quote(C:/Program Files/HDF_Group/HDF5/HDF5-1.8.16-win64/lib/libzlib.lib)
LIBS += $$quote(C:/Program Files/HDF_Group/HDF5/HDF5-1.8.16-win64/lib/libhdf5.lib)
LIBS += $$quote(C:/Program Files/HDF_Group/HDF5/HDF5-1.8.16-win64/lib/libhdf5_cpp.lib)
#LIBS += $$quote(C:/karen/STFC/Technical/HDF5/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/build/_CPack_Packages/win64/ZIP/HDF5-1.8.16-win64/lib/libszip.lib)
#LIBS += $$quote(C:/karen/STFC/Technical/HDF5/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/build/_CPack_Packages/win64/ZIP/HDF5-1.8.16-win64/lib/libzlib.lib)
#LIBS += $$quote(C:/karen/STFC/Technical/HDF5/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/build/_CPack_Packages/win64/ZIP/HDF5-1.8.16-win64/lib/libhdf5.lib)
#LIBS += $$quote(C:/karen/STFC/Technical/HDF5/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/build/_CPack_Packages/win64/ZIP/HDF5-1.8.16-win64/lib/libhdf5_cpp.lib)
#INCLUDEPATH += "C:/Program Files/HDF_Group/HDF5/1.8.16/include"
INCLUDEPATH += "C:/Program Files/HDF_Group/HDF5/HDF5-1.8.16-win64/include"
#LIBS += libhdf5_cpp.lib
#INCLUDEPATH += "C:/karen/STFC/Technical/HDF5/CMake-hdf5-1.8.16/CMake-hdf5-1.8.16/build/_CPack_Packages/win64/ZIP/HDF5-1.8.16-win64/include"

HEADERS += \
    filereader.h
