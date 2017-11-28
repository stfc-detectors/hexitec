
-----------------------------------------------
Instructions for running HexitecGigE executable 
-----------------------------------------------

Running the executable from within QT creator:
----------------------------------------------

In order to run the executable, a total of 11 DLL files are required within the HexitecGigE/release folder. These files are:


19/03/2015  14:11           125,136 EbTransportLayerLib64.dll
19/03/2015  14:10           210,640 EbUtilsLib64.dll
03/05/2017  13:32           204,800 GigE.dll

19/03/2015  14:14           579,280 PtConvertersLib64.dll
19/03/2015  14:09           154,320 PtUtilsLib64.dll
19/03/2015  14:25           179,920 PvBase64.dll
19/03/2015  14:25           202,448 PvBuffer64.dll
19/03/2015  14:25           423,632 PvDevice64.dll
19/03/2015  14:25           394,960 PvGenICam64.dll
19/03/2015  14:25           212,176 PvSerial64.dll
19/03/2015  14:25           328,912 PvStream64.dll

Running the executable standalone: (i.e. without QT creator)
------------------------------------------------------------

In addition to the 11 DLL files specified above, The Windows Deploy Tool of QT must also be run to generate some additional files.

1) Open the QT developer command prompt.

2) Navigate to the folder containing the HexitecGigE.exe file

3) Execute the tool, specifying the full path: 
(note that this is for QT 5.6, amend the path according to your version of QT)

c:\Qt56\5.6\msvc2015_64\bin\windeployqt.exe HexitecGigE.exe

