
---------------------------------------------------
Instructions for running the HexitecGigE executable 
---------------------------------------------------

Prerequisites

aSpect dll, version 1.0.1.9
Pleora:
eBus runtime 64-bit 5.0.0.4100

Rather confusingly, the 32 bit version of the aSpect DLL should be copied to the C:\Windows\syswow64 folder, whereas the 64-bit version should be copied to C:\Windows\system32 folder. Please follow those instructions to the letter. The GigE.dll version should match to version of the message DLL, so also 1.0.1.9. This can be found in the HexitecGigE/ subfolder. The version of the message DLL requires version 5 of the Pleora drivers, as outlined above. Previous version of these DLLs required on older version of the Pleora drivers (4.0.8.3423). So be careful checking DLL versions and Pleora driver versions if a pre-December 2017 version of HexitecGigE is to be run.

At the moment, there isn't enough space to fit them here - please liaise with Matt Wilson for a copy of the message DLL and the Pleora drivers.

-----------------------------------------------------
Instructions for building the HexitecGigE source code
-----------------------------------------------------

The executable resulting from building the source code was built on Win 7, using Qt 5.6.2 (64 bit), Microsoft Visual Studio 2015 Professional and Windows SDK version 10.0.10586.0. It has also been confirmed building on Win 10, Qt 5.9.2, Microsoft Visual Studio 2017 Community Edition, Windows SDK 10.0.16299.0. 


Building the various projects:
------------------------------

It is recommended to build the different projects in this order:

ConfigureHexitecGigE
DetectorLib
HxtProcessingLib
SerialPortLib

With these project built, the HexitecGigE project will have its' dependent libraries available and can be successfully built. Note that its' profile file (HexitecGigE.pro) may need modifying. It relies upon a Windows environment variable to pick up the correct paths etc.
 The most recent build environment is reflected for the variable value of: 

ckd_devel

Note that the project HxtProcessingLibTest isn't a dependency upon HexitecGigE. However, running its executable allows processing of data without using the GUI. This has proven useful for debugging purposes in the past. ConfigHexitecGigE is the tool used to define where the two configuration .ini files are located, along with toggling the Data Acquisition tabs on/off within the GUI. Example ini files, along with calibration files, can be found in IniFiles/.


Running the executable standalone: (i.e. without QT creator)
------------------------------------------------------------

In order to run the executable, the GigE.dll file found in the HexitecGigE/ folder must be present in the folder containing the HexitecGigE.exe file, e.g. the HexitecGigE/release/ folder.

In addition, The Windows Deploy Tool of QT must also be run to generate some additional files.

1) Open the QT developer command prompt.

2) Navigate to the folder containing the HexitecGigE.exe file

3) Execute the tool, specifying the full path: 

(note that this is for QT 5.6, amend the path according to your version of QT)

c:\Qt56\5.6\msvc2015_64\bin\windeployqt.exe HexitecGigE.exe


Instructions prepared by Christian Angelsen, 08/12/2017.
