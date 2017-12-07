
-----------------------------------------------
Instructions for running the HexitecGigE executable 
-----------------------------------------------

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

Note that the project HxtProcessingLibTest isn't a dependency upon HexitecGigE. However, running its executable allows processing of data without using the GUI. This has proven useful for debugging purposes in the past. ConfigHexitecGigE is the tool used to define where the two configuration .ini files are located, along with toggling the Data Acquisition tabs on/off within the GUI.


Running the executable standalone: (i.e. without QT creator)
------------------------------------------------------------

In order to run the executable, the GigE.dll file found in the HexitecGigE/ folder must be present in the folder containing the HexitecGigE.exe file, e.g. the HexitecGigE/release/ folder.

In addition, The Windows Deploy Tool of QT must also be run to generate some additional files.

1) Open the QT developer command prompt.

2) Navigate to the folder containing the HexitecGigE.exe file

3) Execute the tool, specifying the full path: 

(note that this is for QT 5.6, amend the path according to your version of QT)

c:\Qt56\5.6\msvc2015_64\bin\windeployqt.exe HexitecGigE.exe

 