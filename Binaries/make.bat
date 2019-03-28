@ECHO OFF
if NOT "%VCToolsInstallDir%"=="" goto compile
if "%VCINSTALLDIR%"=="" call "%VS150COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS140COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS100COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS110COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS120COMNTOOLS%vsvars32.bat"
:compile
@nmake /nologo %1
