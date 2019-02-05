
---------------------------------------------------
Instructions for running the HexitecGigE executable 
---------------------------------------------------

Prerequisites

aSpect dll, version 1.0.1.9
Pleora:
eBus runtime 64-bit 5.0.0.4100

Rather confusingly, the 32 bit version of the aSpect DLL should be copied to the C:\Windows\syswow64 folder, whereas the 64-bit version should be copied to C:\Windows\system32 folder. Please follow these instructions to the letter. The GigE.dll version should match the version of the message DLL, so also 1.0.1.9. This can be found in the HexitecGigE/ subfolder. The version of the message DLL requires version 5 of the Pleora drivers, as outlined above. Previous version of these DLLs required an older version of the Pleora drivers (4.0.8.3423). So be careful checking DLL versions and Pleora driver versions if a pre-December 2017 version of HexitecGigE is to be run.

At the moment, there isn't enough space to fit them here - please liaise with Matt Wilson for a copy of the message DLL and the Pleora drivers.

-----------------------------------------------------
Instructions for building the HexitecGigE source code
-----------------------------------------------------

The executable resulting from building the source code was built on Win 7, using Qt 5.6.2 (64 bit), Microsoft Visual Studio 2015 Professional and Windows SDK version 10.0.10586.0. Here's a table of confirmed working tool chains:

Windows:       	Qt:              MSVC:                                           Win SDK:
Win 7,         	5.6.2 (64 bit)  Microsoft Visual Studio 2015 Professional,      SDK version 10.0.10586.0 
Win 10,        	5.9.2,          Microsoft Visual Studio 2017 Community Edition, SDK version 10.0.16299.0
Win 7,         	5.9.2,          Microsoft Visual Studio 2017 Professional,      SDK version 10.0.16299.0
Win 10,		5.9.2,		Microsoft Visual Studio 2017 Professional,	SDK version 10.0.17763.0

Building the various projects:
------------------------------

It is recommended to build the different projects in this order:

ConfigureHexitecGigE
DetectorLib
HxtProcessingLib
SerialPortLib

With these project built, the HexitecGigE project will have its' dependent libraries available and can be successfully built. 

PLEASE NOTE: The profile file (HexitecGigE.pro) WILL REQUIRE modifying if the project hasn't been successfully built previous on the same machine. It relies upon a Windows System Environment variable to pick up the correct paths etc.
 If HEXITECGIGE_SYSTEM doesn't exist, create a system variable and give it a suitable value. As of February 2018, the most recent build environment is reflected as:

HEXITECGIGE_SYSTEM=ckd_devel

Look in the profile file near the end for the paragraph those beginning contains "ckd_devel".


HexitecGigE also relies upon a deprecated Qt module called QtScript. It is best to include it whilst installing QT for the first time. It may be possible to add it later but if this doesn't work, simply uninstall all of QT and reinstall with QtScript included. The instructions for adding this library with Qt already installed:
Run the QT maintenance tool which is typically can be found at: C:\Qt\MaintenanceTool.exe. Select "Add or remove components", click "Next" then for the installed Qt Version (e.g. 5.9.2), expend that section and ensure "Qt Script (Deprecated)" is ticked before clicking "Next" followed by "Update". Built successfully on Windows 10, the Eigen library dependency was updated to version 3.3.7.


Note that the project HxtProcessingLibTest isn't a dependency upon HexitecGigE. However, running its executable allows processing of data without using the GUI. This has proven useful for debugging purposes in the past. ConfigHexitecGigE is the tool used to define where the two configuration .ini files are located, along with toggling the Data Acquisition tabs on/off within the GUI. Example ini files, along with calibration files, can be found in IniFiles/.

The following projects are only included in the repository for legacy purposes and it is not necessary to attempt to build any of these projects:

Configure2Easy/
HxtProcessingLib_CA/
TemperatureHumidityLib/
Translator/
XpsLib/


Running the executable standalone: (i.e. without QT creator)
------------------------------------------------------------

In order to run the executable, the GigE.dll file found in the HexitecGigE/ folder must be present in the folder containing the HexitecGigE.exe file, e.g. the HexitecGigE/release/ folder.

In addition, The Windows Deploy Tool of QT must also be run to generate some additional files.

1) Open the QT developer command prompt.

2) Navigate to the folder containing the HexitecGigE.exe file

3) Execute the tool, specifying the full path: 

(note that these notes were originally prepared on QT 5.6, amend the path according to your version of QT)

c:\Qt\5.6\msvc2015_64\bin\windeployqt.exe HexitecGigE.exe


Instructions prepared by Christian Angelsen, 23/02/2018. 
Amendments made on 05/02/2019 - Added information related to Windows 10, legacy projects.

